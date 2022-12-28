#!/bin/sh

# insert required external tools here
reqbins="nc openssl"

# private and public key paths
prkey="id_rsa"
pubkey="id_rsa.pub"

# remote public key path
rpubkey="rid_rsa.pub"

# log in stderr
eecho() {
	text=$1
	echo $text >&2
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

