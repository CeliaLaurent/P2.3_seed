P2.3 - The Finite Element Method Using deal.II
===============================================
## Lecturer: Luca Heltai

This repository contains the assignments and workspaces for the
course P2.3,  The Finite Element Method Using deal.II

Running deal.II using docker
============================

```
docker run -t -i dealii/dealii:latest

```

If you add the flag -u0, then you will be running the docker image as root in order to install new packages.

Google doc notes
================

https://bit.ly/2WgfTXZ :

> sudo add-apt-repository ppa:ginggs/deal.ii-backports
>
> sudo apt-get update
>
> sudo apt-get install libdeal.ii-dev
>
> 
>
> **Eclipse:**
>
> https://github.com/dealii/dealii/wiki/Eclipse
>
> 
>
> -DCMAKE_PREFIX_PATH=/Applications/deal.II.app/Contents/Resources/Libraries 
>
> -DDEAL_II_DIR=/Applications/deal.II.app/Contents/Resources/
>
> 
>
> **Using spack:**
>
> [**https://github.com/dealii/dealii/wiki/deal.II-in-Spack**](https://github.com/dealii/dealii/wiki/deal.II-in-Spack)

License
=======
The course content for _P2.4 - The Finite Element Method Using deal.II_ is licensed under a
Creative Commons Attribution-ShareAlike 4.0 International License.

You should have received a copy of the license along with this
work. If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.
