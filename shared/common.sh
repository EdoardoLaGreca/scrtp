#!/bin/sh

# put all the common things here to avoid duplication in scrtp and scrtpd
# it may happen that some pieces of code are identical for scrtp and scrtpd but still they are left duplicated. this happens when those
# pieces of code may be changed in future in a way that makes them different between each other

# required external tools
reqbins=$(cat reqbins)

# private and public key paths
prkey="id_rsa"
pubkey="id_rsa.pub"

# log in stderr
eecho() {
	text=$1
	echo $text >&2
}

# send through udp
udpsend() {
	payload=$1
	echo "$payload" | nc -u -q 0 $host $port
}

# receive through udp
udprecv() {
	nc -lu -W 1 $host $port
}

[ "$reqbins" ] && ../script/chkbins.sh $reqbins || exit 1

# generate keys if missing
if [ ! -r $prkey ]
then
	# write private key
	openssl genpkey -algorithm RSA -out $prkey

	# write public key
	openssl rsa -in $prkey -pubout >$pubkey
fi

# check if remote public key is present
if [ ! -r $rpubkey ]
then
	eecho "$0: missing remote public key $rpubkey"
	exit 1
fi
