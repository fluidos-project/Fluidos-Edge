// Note: the example only works with the code within the same release/branch.
package main

/* Requirement: Install client-go
*  go get k8s.io/client-go@kubernetes-1.24.0
*  go get github.com/kubeedge/kubeedge/tests/e2e/utils@v1.14.3
*  go get edge-resource-manager/utils_fe
*  go mod vendor
 */

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"

	"context"

	v1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"

	b64 "encoding/base64"

	edgeclientset "github.com/kubeedge/kubeedge/pkg/client/clientset/versioned"
	"github.com/kubeedge/kubeedge/tests/e2e/utils"
)

func main() {

	// Build kubeconfig path
	var kubeconfig *string
	// if home := homedir.HomeDir(); home != "" {
	if home, err := os.Getwd(); home != "" && err == nil {
		kubeconfig = flag.String("kubeconfig", filepath.Join(home, ".kube", "config"), "")
	} else {
		kubeconfig = flag.String("kubeconfig", "/home/othon/Desktop/Projects/FLUIDOS/SW/go-sources/edge-resource-manager/.kube/config", "")
	}
	flag.Parse()

	// Create new client to communicate with K8S API server
	var edgeClientSet edgeclientset.Interface = utils.NewKubeEdgeClient(*kubeconfig)

	// Get edge nodes list
	// path-to-kubeconfig -- for example, /root/.kube/config
	config, _ := clientcmd.BuildConfigFromFlags("", *kubeconfig)
	// creates the clientset
	clientset, _ := kubernetes.NewForConfig(config)
	// access the API to list pods

	nodes, _ := clientset.CoreV1().Nodes().List(context.TODO(), v1.ListOptions{
		LabelSelector: "node-role.kubernetes.io/edge=",
	})

	fmt.Printf("\033[7mTotal Egde Nodes found:\033[0m %d\n", len(nodes.Items))

	items := nodes.Items
	for i, item := range items {
		fmt.Printf("%d. Name: %10s, Arch: %s\n", i+1, item.Name,
			item.Status.NodeInfo.Architecture) //,
	}

	fmt.Println()

	// Get device list
	// FIXME: No error check
	deviceInstanceList, _ := utils.ListDevice(edgeClientSet, "default")

	fmt.Printf("\033[7mTotal devices found:\033[0m %d\n", len(deviceInstanceList))

	for idx, device := range deviceInstanceList {
		fmt.Printf("\033[0m[Device %d]\033[0m\n", idx)

		fmt.Printf("\033[0mName:\033[0m %s\n", device.Name)

		desc := device.ObjectMeta.Labels["description"]
		fmt.Printf("\033[0mDescription:\033[0m %s\n", desc)

		manu := device.ObjectMeta.Labels["manufacturer"]
		fmt.Printf("\033[0mManufacturer:\033[0m %s\n", manu)

		model := device.ObjectMeta.Labels["model"]
		fmt.Printf("\033[0mModel:\033[0m %s\n", model)

		sensorsBase64 := device.ObjectMeta.Annotations["sensors"]
		// fmt.Printf("\033[0mSensors Base64 Encoded:\033[0m %s\n", sensorsBase64)

		sensorsDec, _ := b64.StdEncoding.DecodeString(sensorsBase64)
		fmt.Printf("\033[0mSensors Decoded\033[0m \n %s\n", sensorsDec)
	}
}
