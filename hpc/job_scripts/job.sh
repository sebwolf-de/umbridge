#! /bin/bash
TZ='Europe/Berlin' date

# Launch model server, send back server URL,
# then wait to keep the server alive.

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
export RANKS=$(wc -l $MACHINE_FILE | awk '{print $1}')

### TODO adapt the path here
#cd $WORK/UQ/Seis-Bridge/tpv13
#python3 tpv13server.py &

cd $HOME/models/
python3 loglikelihood_gauss_1_1.py -hq &

### TODO adapt the path here
load_balancer_dir="${HOME}/umbridge/hpc"

host=$(hostname -I | awk '{print $1}')

echo "http://${host}:${port}"
# Wait for model server to start
while ! curl -s "http://$host:$port/Info" > /dev/null; do
    sleep 1
done

echo "Started server successfully, running on ${host}:${port}"

# Write server URL to file identified by SERVER_ID.
mkdir -p "$load_balancer_dir/urls"
echo "http://$host:$port" > "$load_balancer_dir/urls/url-$SERVER_ID.txt"

sleep infinity # keep the job occupied
