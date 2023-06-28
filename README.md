# NDPmulator

NDPmulator is a framework built on top of gem5 targeting the fast modeling, validation, and evaluation of novel Near-Data Processing architectures before actually implementing them in RTL. To achieve this, three sets of tools are provided:
* An [architectural model](#architectural-model) consisting of a boilerplate that automatically takes care of integration with the CPU and the memory hierarchy, as well as data management.
* [Simulation scripts](#simulation-scripts) targeting both SE and FS mode, exemplifying how to set up an NDP-enabled processing system that can be simulated by gem5. In addition, a (template) Linux driver is also provided to enable control of the NDP device from the OS when FS mode is used.
* Finally, an example [host code](#host-code) is also provided to show how to interact and control the NDP device from the host side. This example is compatible both with the SE mode (directly) and the FS mode (through the provided device driver).

## Architectural model

The architectural model of NDPmulator is illustrated below. As it can be observed, three main architectural blocks are provided to help in the modeling of novel NDP devices: (1) System address management; (2) Prog. IO; and (3) Load/Store Unit.

![architectural model](figs/architectural_model.jpg)

The System address management block is responsible for managing a shared memory region that can be accessed by the CPU and the NDP device. The NDP device reads the operands and stores the results within this region.

The Prog. IO block implements a bank of programming registers that can be used by the CPU to control the NDP device.

Finally, the Load and Store Unit is responsible for mediating all data transfers between the NDP device and the memory hierarchy, automatically implementing and concealing all the complex control required for that effect.

The architectural model can be found [here](src/ndp), and a practical example of an NDP device based on the provided architectural model is [here](src/ndp_dev_a). 

## Simulation scripts

Examples of both SE and FS simulation scripts can be found [here](configs/ndp).

## Host code

An example illustrating how to communicate with the NDP device from the host code can be found [here](tests/test-progs/ndp/ndp_dev_a), together with a device driver to be used with FS mode.

## FS kernels and images

The authors rely on the images and kernels officially supported by gem5, available at https://resources.gem5.org/.

## How to cite

```
@inproceedings{DBLP:conf/sbac-pad/VieiraR0T22,
  author       = {Jo{\~{a}}o Vieira and
                  Nuno Roma and
                  Gabriel Falc{\~{a}}o and
                  Pedro Tom{\'{a}}s},
  title        = {gem5-ndp: Near-Data Processing Architecture Simulation From Low Level Caches to {DRAM}},
  booktitle    = {{SBAC-PAD}},
  pages        = {41--50},
  publisher    = {{IEEE}},
  year         = {2022}
}
```
