.. This work is licensed under a Creative Commons Attribution 4.0 International License.
.. http://creativecommons.org/licenses/by/4.0
.. (c) <optionally add copywriters name>

============
Introduction
============
.. In this section explain the purpose of the scenario and the types of capabilities provided

The k8-nosdn-stor4nfv-ha is intended to be used to install the OPNFV Stor4NFV project in a standard
OPNFV High Availability mode. The OPNFV Stor4NFV project integrates the OpenSDS and Ceph projects
into the OPNFV environment.

Scenario components and composition
===================================
.. In this section describe the unique components that make up the scenario,
.. what each component provides and why it has been included in order
.. to communicate to the user the capabilities available in this scenario.

This scenario installs everything needed to use the Stor4NFV project in an OPNFV
environment. Mainly two upstream projects, including OpenSDS and Ceph, will be installed
and deployed.

Ceph plays a role as the data plane and the backend driver of OpenSDS. 'dock' service of
OpenSDS need to be deployed in Ceph monitor node. The users can use OpenSDS, control plane,
to create, inquire, and delete storage resources.

Scenario usage overview
=======================
.. Provide a brief overview on how to use the scenario and the features available to the
.. user.  This should be an "introduction" to the userguide document, and explicitly link to it,
.. where the specifics of the features are covered including examples and API's

Once this scenario is installed, it will be possible for kubernetes to create volume through
OpenSDS API to call Ceph, and in this case, Ceph will be used for storage backend together
with OpenSDS.

Limitations, Issues and Workarounds
===================================
.. Explain scenario limitations here, this should be at a design level rather than discussing
.. faults or bugs.  If the system design only provide some expected functionality then provide
.. some insight at this point.

References
==========

For more information about Stor4NFV, please visit

https://wiki.opnfv.org/display/PROJ/Stor4NFV

https://wiki.opnfv.org/display/STOR

For more information on the OPNFV Gambia release, please visit

http://www.opnfv.org/gambia
