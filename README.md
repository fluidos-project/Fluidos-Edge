# Introduction

This repository outlines the steps to create a basic setup for managing Edge nodes and devices, and is designed to meet [Fluidos project](https://fluidos.eu) requirements. It includes the installation and configuration of various components, such as, Kubernetes, CloudCore, EdgeCore, etc. These steps enable easy access and management of Edge nodes using standard Kubernetes methods from a Fluidos node.

## Repository structure



## How to use

 In order to ```clone``` this repository with the submodules includes, use the command:
```bash
git clone --recurse-submodules https://github.com/otto-tom/Fluidos-Edge.git
```
If you do not intend to build the Fluidos Edge System or the Mappers from source you can omit these submodules by using the following command:
```bash
git clone https://github.com/otto-tom/Fluidos-Edge.git
``` 
If you have already cloned only the basic repository, and you wish to build the submodules, use this command:
```bash
git submodule update --init --progress
```
Alternatively, if you wish to work only with the Fluidos Edge System submodule:
```bash
git submodule update --init --progress sw/FluidosEdge-System
```
Or if you wish to work only with the Fluidos Edge Mappers  submodule:
```bash
git submodule update --init --progress sw/FluidosEdge-Mappers
```