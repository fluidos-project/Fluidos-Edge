#!/usr/bin/bash

GREEN="32"
#BBCOLOR="\e[1;4;${GREEN}m\e[100m"
#BBCOLOR="\e[1;${GREEN}m\e[100m"
BBCOLOR="\e[1m\e[42m"
#BCOLOR="\e[1;4;${GREEN}m"
BCOLOR="\e[1;${GREEN}m"
BTEXT="\e[1m"
ENDFORMAT="\e[0m"
NOOUT=/dev/null
WITHOUT=/dev/stdout
OUTPUT=$NOOUT

set -euo pipefail

echo -ne "${BTEXT}Enable edge worker nodes support for the Fluidos provider cluster? ${ENDFORMAT}[Y/n]: "
read -p "" edge_ena
edge_ena=${edge_ena:-Y}

if [ $edge_ena == "y" -o $edge_ena == "Y" ]; then
  edge_ena=1
else
  edge_ena=0
fi

consumer_node_port=30000
provider_node_port=30001

echo -e "${BCOLOR}Creating FLUIDOS consumer cluster${ENDFORMAT}"
kind create cluster --config consumer/cluster-multi-worker.yaml  --name fluidos-consumer --kubeconfig "$PWD/consumer/config"
if [ $edge_ena -eq 1 ]; then
 echo -e "${BCOLOR}Creating FLUIDOS Edge provider cluster${ENDFORMAT}"
 export KUBECONFIG=$PWD/provider/config
 ./bin/keink create kubeedge --config provider/cluster-multi-worker-edge.yaml  --name fluidos-provider --kubeconfig "$PWD/provider/config" --image othontom/node:v1.14.5-fluidos --wait 120s
else
  echo -e "${BCOLOR}Creating FLUIDOS provider cluster${ENDFORMAT}"
  kind create cluster --config provider/cluster-multi-worker.yaml  --name fluidos-provider --kubeconfig "$PWD/provider/config"
fi

consumer_controlplane_ip=$(docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' fluidos-consumer-control-plane)
provider_controlplane_ip=$(docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' fluidos-provider-control-plane)

echo -e "${BCOLOR}Add and update fluidos-project Helm repository${ENDFORMAT}"
helm repo add fluidos https://fluidos-project.github.io/node/
helm repo update

echo -e "${BCOLOR}Configure consumer cluster${ENDFORMAT}"
export KUBECONFIG=$PWD/consumer/config
echo -ne "${BCOLOR}Apply Fluidos Node CRDs...${ENDFORMAT}"
kubectl apply -f "$PWD/deployments/node/crds" 1> $OUTPUT
echo -e "${BBCOLOR}OK${ENDFORMAT}"
echo -ne "${BCOLOR}Deploy metrics server...${ENDFORMAT}"
kubectl apply -f "$PWD/metrics-server.yaml" 1> $OUTPUT
echo -e "${BBCOLOR}OK${ENDFORMAT}"

echo "Waiting for metrics-server to be ready"
kubectl wait --for=condition=ready pod -l k8s-app=metrics-server -n kube-system --timeout=360s   

echo -e "${BCOLOR}Install Liqo${ENDFORMAT}"
liqoctl install kind --cluster-name fluidos-consumer #\
#  --set controllerManager.config.resourcePluginAddress=node-rear-controller-grpc.fluidos:2710 \
#  --set controllerManager.config.enableResourceEnforcement=true

echo -e "${BCOLOR}Deploy Fluidos Node components${ENDFORMAT}"
helm install node fluidos/node --version "v0.0.6" -n fluidos \
  --create-namespace -f consumer/values.yaml \
  --set "provider=kind" \
  --set networkManager.configMaps.nodeIdentity.ip="$consumer_controlplane_ip:$consumer_node_port" \
  --set networkManager.configMaps.providers.local="$provider_controlplane_ip:$provider_node_port" \
  --wait

echo -e "${BCOLOR}Configure provider cluster${ENDFORMAT}"
export KUBECONFIG=$PWD/provider/config

if [ $edge_ena -eq 1 ]; then
  echo -ne "${BCOLOR}Patch K8S server to exclude the edge worker node from executing kube-proxy...${ENDFORMAT}"
  kubectl patch daemonset kube-proxy --context kind-fluidos-provider -n kube-system -p '{"spec": {"template": {"spec": {"affinity": {"nodeAffinity": {"requiredDuringSchedulingIgnoredDuringExecution": {"nodeSelectorTerms": [{"matchExpressions": [{"key": "node-role.kubernetes.io/edge", "operator": "DoesNotExist"}]}]}}}}}}}' 1> $OUTPUT
  echo -e "${BBCOLOR}OK${ENDFORMAT}"
  echo -ne "${BCOLOR}Patch K8S server to exclude the edge worker node from executing coredns...${ENDFORMAT}"
  kubectl patch deploy coredns --context kind-fluidos-provider -n kube-system -p '{"spec": {"template": {"spec": {"affinity": {"nodeAffinity": {"requiredDuringSchedulingIgnoredDuringExecution": {"nodeSelectorTerms": [{"matchExpressions": [{"key": "node-role.kubernetes.io/edge", "operator": "DoesNotExist"}]}]}}}}}}}' 1> $OUTPUT
  echo -e "${BBCOLOR}OK${ENDFORMAT}"
  echo -ne "${BCOLOR}Apply CloudCore CRDs...${ENDFORMAT}"
  # Apply CloudCore CRDs
  kubectl apply -f "$PWD/deployments/edge/cloudcore/crds" &> $OUTPUT
  echo -e "${BBCOLOR}OK${ENDFORMAT}"
fi

echo -ne "${BCOLOR}Apply Fluidos Node CRDs...${ENDFORMAT}"
kubectl apply -f "$PWD/deployments/node/crds" 1> $OUTPUT
echo -e "${BBCOLOR}OK${ENDFORMAT}"
echo -ne "${BCOLOR}Deploy Metrics server...${ENDFORMAT}"
kubectl apply -f "$PWD/metrics-server.yaml" 1> $OUTPUT
echo -e "${BBCOLOR}OK${ENDFORMAT}"

echo "Waiting for metrics-server to be ready"
kubectl wait --for=condition=ready pod -l k8s-app=metrics-server -n kube-system --timeout=360s

echo -e "${BCOLOR}Install Liqo${ENDFORMAT}"
liqoctl install kind --cluster-name fluidos-provider #\
#  --set controllerManager.config.resourcePluginAddress=node-rear-controller-grpc.fluidos:2710 \
#  --set controllerManager.config.enableResourceEnforcement=true

echo -e "${BCOLOR}Install Fluidos Node components${ENDFORMAT}"
helm install node fluidos/node --version "v0.0.6" -n fluidos \
  --create-namespace -f provider/values.yaml \
  --set "provider=kind" \
  --set networkManager.configMaps.nodeIdentity.ip="$provider_controlplane_ip:$provider_node_port" \
  --set networkManager.configMaps.providers.local="$consumer_controlplane_ip:$consumer_node_port" \
  --wait

if [ $edge_ena -eq 1 ]; then
  echo -ne "${BCOLOR}Patch K8S server to exclude the edge worker node from executing liqo-route...${ENDFORMAT}"
  kubectl patch daemonset liqo-route --context kind-fluidos-provider -n liqo -p '{"spec": {"template": {"spec": {"affinity": {"nodeAffinity": {"requiredDuringSchedulingIgnoredDuringExecution": {"nodeSelectorTerms": [{"matchExpressions": [{"key": "node-role.kubernetes.io/edge", "operator": "DoesNotExist"}]}]}}}}}}}' 1> $OUTPUT
  echo -e "${BBCOLOR}OK${ENDFORMAT}"
  echo -ne "${BCOLOR}Patch K8S server to exclude the edge worker node from executing liqo-telemetry...${ENDFORMAT}"
  kubectl patch cronjob.batch liqo-telemetry --context kind-fluidos-provider -n liqo -p '{"spec": {"jobTemplate": {"spec": {"template": {"spec": {"affinity": {"nodeAffinity": {"requiredDuringSchedulingIgnoredDuringExecution": {"nodeSelectorTerms": [{"matchExpressions": [{"key": "node-role.kubernetes.io/edge", "operator": "DoesNotExist"}]}]}}}}}}}}}' 1> $OUTPUT
  echo -e "${BBCOLOR}OK${ENDFORMAT}"

  echo -ne "${BCOLOR}Taint edge worker node to exclude from scheduling workload...${ENDFORMAT}"
  kubectl taint nodes fluidos-provider-worker2 key=NoSchedule:NoSchedule 1> $OUTPUT
  echo -e "${BBCOLOR}OK${ENDFORMAT}"

  echo -ne "${BCOLOR}Setup the MQTT broker at the edge worker node...${ENDFORMAT}"
  worker_node=$(docker ps --filter "name=fluidos-provider-worker2" -q)
  docker exec --privileged -it $worker_node apt-get update 1> $OUTPUT
  docker exec --privileged -it $worker_node apt-get install -y mosquitto 1> $OUTPUT
  docker cp "$PWD/provider/mqtt_access.conf" $worker_node:/etc/mosquitto/conf.d 1> $OUTPUT
  docker exec --privileged -it $worker_node systemctl enable mosquitto 1> $OUTPUT
  docker exec --privileged -it $worker_node systemctl start mosquitto 1> $OUTPUT
  echo -e "${BBCOLOR}OK${ENDFORMAT}"
fi

echo -e "${BBCOLOR}Fluidos Edge Testbed successfully completed${ENDFORMAT}"
