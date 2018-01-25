# DMR2SIP

Integrate DMR radio stations with SIP software.

### Installation

Install the dependencies
```sh
    # apt-get update 
    # apt-get install git make docker.io alsa
```
Prepare docker to work
```sh
    # usermod -aG docker <USER> (need to relogin)
 ```

Create all DMR2SIP components
```sh
    $ make
```
Start all DMR2SIP components
```sh
    $ make start
```
Set autorun for DMR2SIP
```sh
    $ make install
```

Create web console (frontend, backend, db) docker containers
```sh
    $ make wc
```
Create service docker container and compile service program
```sh
    $ make service
```
