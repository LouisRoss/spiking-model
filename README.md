# Spiking Neuron Model
## A C++ Spiking Neural Network based on [LouisRoss/ModelEngine](https://github.com/LouisRoss/modelengine) to efficiently execute with all available cores

This program is designed to run *spiking neural networks* (SNNs) using the [Model Engine](https://github.com/LouisRoss/modelengine)
framework.  SNNs are different from the more common *artificial neural networks* typically used in today's deep learning algorithms.

ANNs capture a static snapshot of the average firing rate of neurons.  Each neuron's state is a single positive value indicating its activity.  Connections between neurons are captured as another single number, a weight, this time a signed value that can swing positive or negative.  The activity of a downstream neuron is a function of summing all the products of the activities of upstream neurons multiplied by their connections.

SNNs, are an attempt to be more biologically plausible.  As with ANNs, there are upstream neurons that connect with downstream neurons, and the connections have weights.  The difference is that with SNNs there is no averaging &mdash; each neuron produces a spike or series of spikes.  Downstream neurons integrate the spikes sent by upstream neurons, as mediated by the connection weights, and when enough recent upstream activity accumulates, the downstream neuron will fire its own spike.

Reasons why spiking might be worth trying:
- It is possible that the spike timing and phase of arriving spikes adds information that gets lost during the averaging process used in ANNs.
- Learning in biological systems uses only information that is local to the networks doing the learning.  In ANNs, learning is typically done through backpropagation of errors, requiring global knowledge of the whole system.
- The fraction of neurons spiking at any time in SNNs is typically quite small, resulting in sparse networks.  This sparsity might provide a compute advantage if only the active neurons need processing time.

This program attempts to make use of sparsity in SNNs to efficiently compute the model using all available cores of a CPU.  One reasong to try this is as a baseline of comparison against a similar algorithm implemented in CUDA on a GPU, which most likely cannot take advantage of the sparsity.

For more discussion of SNNs, see the [wikipedia article](https://en.wikipedia.org/wiki/Spiking_neural_network).

There are many existing frameworks designed to simulate spiking neurons.  Some of these include:
- [Nest](https://github.com/nest/nest-simulator). NEST is a simulator for spiking neural network models that focuses on the dynamics, size and structure of neural systems rather than on the exact morphology of individual neurons.
- [Neuron](https://www.neuron.yale.edu/neuron/).  The NEURON simulation environment is used in laboratories and classrooms around the world for building and using computational models of neurons and networks of neurons.
- [Brian](https://briansimulator.org/).  Brian is a free, open source simulator for spiking neural networks. It is written in the Python programming language and is available on almost all platforms.
- [Brain Modeling Toolkit](https://alleninstitute.github.io/bmtk/).  The Brain Modeling Toolkit (BMTK) is a python-based software package for building, simulating and analyzing large-scale neural network models.

With so many platforms already in place to model SNNs, why build another one?

Basically, the goals of this project are to use the resources of a single CPU (meaning all cores) or GPU to do near-real-time long-running simulations of the largest possible networks.  The hope is that this may eventually run on smaller hardware (think NVidia Jetson Nano, or even ASIC designs) to become the control elements of mobile autonomous systems.

Also, it seems like a lot of fun!

<br> <a name="contents"></a>
Contents
--------

* [License](#license)
* [Getting Started](#getting-started)


## License
-------

Spiking-Model: C++ Spiking Neural Network that efficiently makes use of all available cores.

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The class is licensed under the [MIT License](http://opensource.org/licenses/MIT):

Copyright &copy; 2020 [Louis Ross](louis.ross@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

* * *
The software contains the ModelEngine header-only C++ library from Louis Ross which is licensed under the MIT License. 
Copyright © 2020 Louis Ross louisross@gmail.com

The software contains the Json header-only C++ library from Niels Lohmann which is licensed under the MIT License. 
Copyright © 2013-2019 Niels Lohmann mail@nlohmann.me

The software contains the CSV header-only C++ library from Vincent La which is licensed under the MIT License. 
Copyright © 2017-2019 Vincent La http://vincela.com/

The software contains the HighFive C++ library from the Blue Brain project which is licensed under the Boost Software License. 
Copyright © 2015-2017 Blue Brain Project https://www.epfl.ch/research/domains/bluebrain/

## Getting Started
---------------


