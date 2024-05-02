// Note: the example only works with the code within the same release/branch.
package main

/* Requirement: Install client-go
*  go get k8s.io/client-go@kubernetes-1.24.0
*  go get github.com/kubeedge/kubeedge/tests/e2e/utils@v1.14.3
*  go get edge-resource-manager/utils_fe
*  go mod vendor
 */
/* In file go.mod
 * use(
 *  ./utils_fe
 * )
 */

import (
	"flag"
	"fmt"
	"net/http"
	"os"
	"path/filepath"

	v1ke "github.com/kubeedge/kubeedge/pkg/apis/rules/v1"
	edgeclientset "github.com/kubeedge/kubeedge/pkg/client/clientset/versioned"

	utils_fe "connection_manager/utils_fe"
)

func main() {

	deletePtr := flag.Bool("delete", false, "a bool")
	createPtr := flag.Bool("create", false, "a bool")
	createTargetPtr := flag.String("target", "0.0.0.0", "a string")

	flag.Parse()

	fmt.Println("Connection Manager")

	// Build kubeconfig path
	var kubeconfig *string
	// if home := homedir.HomeDir(); home != "" {
	if home, err := os.Getwd(); home != "" && err == nil {
		kubeconfig = flag.String("kubeconfig", filepath.Join(home, ".kube", "config"), "")
	} else {
		kubeconfig = flag.String("kubeconfig", "/home/othon/Desktop/Projects/FLUIDOS/SW/go-sources/connection_manager/.kube/config", "")
	}
	flag.Parse()

	// Create new client to communicate with K8S API server
	var edgeClientSet edgeclientset.Interface = utils_fe.NewKubeEdgeClient(*kubeconfig)

	if *deletePtr {
		fmt.Printf("Delete any pre-existing rules")
		// Delete any pre-existing rules
		list, err := utils_fe.ListRule(edgeClientSet, "default")
		if err == nil {
			for _, rule := range list {
				err := utils_fe.HandleRule(edgeClientSet, http.MethodDelete, rule.Name, "", "", "")
				if err != nil {
					fmt.Printf("Error deleting rule %v\n", rule.Name)
				}
			}
			fmt.Printf("Done")
		} else {
			fmt.Println("Error listing rules")
		}

		// Delete any pre-existing ruleendpoints
		reList, err := utils_fe.ListRuleEndpoint(edgeClientSet, "default")
		if err == nil {
			for _, ruleendpoint := range reList {
				err := utils_fe.HandleRuleEndpoint(edgeClientSet, http.MethodDelete, ruleendpoint.Name, "")
				if err != nil {
					fmt.Printf("Error deleting rule endpoint %v\n", ruleendpoint.Name)
				}
			}
		} else {
			fmt.Println("Error listing rule endpoints")
		}
	} else if *createPtr {

		// create rest ruleendpoint
		fmt.Printf("Create REST rule endpoint, ")
		err := utils_fe.HandleRuleEndpoint(edgeClientSet, http.MethodPost, "", v1ke.RuleEndpointTypeRest)
		if err != nil {
			fmt.Println("ERROR ", err)
		} else {
			fmt.Println("OK")
		}

		// create eventbus ruleendpoint
		fmt.Printf("Create EventBus rule endpoint, ")
		err = utils_fe.HandleRuleEndpoint(edgeClientSet, http.MethodPost, "", v1ke.RuleEndpointTypeEventBus)
		if err != nil {
			fmt.Println("ERROR ", err)
		} else {
			fmt.Println("OK")
		}

		// target examples
		// http://10.0.2.70:4487/telegraf
		// http://10.244.0.19:8080
		// create rule: eventbus to rest.
		fmt.Printf("Create rule, target %v, ", *createTargetPtr)
		err = utils_fe.HandleRule(edgeClientSet, http.MethodPost, "", v1ke.RuleEndpointTypeEventBus, v1ke.RuleEndpointTypeRest, *createTargetPtr)
		if err != nil {
			fmt.Println("ERROR ", err)
		} else {
			fmt.Println("OK")
		}
	}
}
