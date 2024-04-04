# HPC

This load balancer allows any scaling up UM-Bridge applications to HPC systems. To the client, it behaves like a regular UM-Bridge server, except that it can process concurrent model evaluation requests. 
It orchestrates several servers. When it receives requests, it will queue them until a server instance on the HPC system is available, and then forward evaluation requests to them. To each model server instance, the load balancer in turn appears as a regular UM-Bridge client.

## Installation

Clone the UM-Bridge repository.

```
git clone https://github.com/UM-Bridge/umbridge.git
```

Then navigate to the `hpc` directory.

```
cd umbridge/hpc
```

Finally, compile the load balancer. Depending on your HPC system, you likely have to load a module providing a recent c++ compiler.

```
mkdir build && cd build
cmake ..
make
   ```


## Usage

The load balancer consists of two binaries:

1. **Load Server**

  The server runs in a slurm  or PBS allocation. It will start `N` different umbridge servers, each working on `N` nodes.
  Run the server as 
  ```
  mpirun -n $(expr $N \* $M) ./load-server $N $M
  ```
  Make sure that your batch job has exactly `N x M` MPI slots.
  
   Adapt the configuration in ``hpc/job_scripts/submit.sh`` to your needs.


2. **Configure model job**

   Adapt the configuration in ``hpc/job_scripts/job.sh`` to your needs:
   * Specify what UM-Bridge model server to run,
   * and set the directory of your load balancer binary in `load_balancer_dir`.

   Importantly, the UM-Bridge model server must serve its models at the port specified by the environment variable `PORT`. The value of `PORT` is automatically determined by `job.sh`, avoiding potential conflicts if multiple servers run on the same compute node.
   If the model server runs an MPI job itself, be sure to use `mpirun -hostfile $MACHINE_FILE`.
   The load server stores the hostnames for some subjobs in a file and `$MACHINE_FILE` points to the correct file.


3. **Load Client**

   The `load-client` is supposed to run on the login node. It receives evaluation requests, queues them and forwards them to available workers.
   You have to tell the `load-client` how many forward models can be evaluated in parallel.

   ```
   ./load-client $N
   ```

4. **Connect from client**

   Once running, you can connect to the load balancer from any UM-Bridge client on the login node via `http://localhost:4242`. To the client, it will appear like any other UM-Bridge server, except that it can process concurrent evaluation requests.
   The model name is `QueuingModel`.

## (Optional) Running clients on your own machine while offloading runs to HPC

Alternatively, a client may run on your own device. In order to connect UM-Bridge clients on your machine to the login node, you can create an SSH tunnel to the HPC system.

```
    ssh <username>@hpc.cluster.address -N -f -L 4242:<server hostname>:4242
    # start ssh tunnel
    # -N : do not execute remote command
    # -f : request ssh to go to the background once the ssh connection has been established
```

While the SSH tunnel is running, you can run the client on your own device, and connect it to the load balancer via `http://localhost:4242`.
