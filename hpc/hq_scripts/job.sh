#! /bin/bash

#HQ --resource model=1
#HQ --time-limit=10m
###HQ --stdout ./stdout
###HQ --stderr ./stderr
#HQ --nodes 2

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
module load tacc-apptainer 
cd $WORK/UQ/Seis-Bridge/tpv5
module load intel
module load python3/3.7.0
which python3
ls -la
python3 ../server/server.py &

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
