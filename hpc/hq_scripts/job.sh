#! /bin/bash
### TODO Set the same number as $M in allocation_queue.sh here.
#HQ --nodes 144
TZ='Europe/Berlin' date

# Launch model server, send back server URL
# and wait to ensure that HQ won't schedule any more jobs to this allocation.

function get_avaliable_port {
    # Define the range of ports to select from
    MIN_PORT=1024
    MAX_PORT=65535

    # Generate a random port number
    port=$(shuf -i $MIN_PORT-$MAX_PORT -n 1)

    # Check if the port is in use
    while lsof -Pi :$port -sTCP:LISTEN -t >/dev/null; do
        # If the port is in use, generate a new random port number
        port=$(shuf -i $MIN_PORT-$MAX_PORT -n 1)
    done

    echo $port
}

port=$(get_avaliable_port)
export PORT=$port
export RANKS=$(wc -l $HQ_NODE_FILE | awk '{print $1}')

# Assume that server sets the port according to the environment variable 'PORT'.
export TACC_IBRUN_DEBUG=1
unset OMPI_MCA_plm_slurm_args
unset PRTE_MCA_plm_slurm_args
unset HYDRA_LAUNCHER_EXTRA_ARGS
unset I_MPI_HYDRA_BOOTSTRAP_EXEC_EXTRA_ARGS
export I_MPI_HYDRA_BOOTSTRAP=ssh
export NODE_TASKS_PPN_INFO=1,0_
mpiexec.hydra -np $RANKS -machinefile $HQ_NODE_FILE /usr/bin/hostname

module load tacc-apptainer 
module load intel
module load python3/3.9.2
### TODO adapt the path here
cd $WORK/UQ/Seis-Bridge/tpv5
ls -la
python3 ../server/server.py &

### TODO adpat the path here
load_balancer_dir="${HOME}/UQ/umbridge/hpc"

host=$(hostname -I | awk '{print $1}')

# Wait for model server to start
while ! curl -s "http://$host:$port/Info" > /dev/null; do
    sleep 1
done

# Write server URL to file identified by HQ job ID.
mkdir -p "$load_balancer_dir/urls"
echo "http://$host:$port" > "$load_balancer_dir/urls/url-$HQ_JOB_ID.txt"

sleep infinity # keep the job occupied
