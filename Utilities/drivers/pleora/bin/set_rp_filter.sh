#!/bin/sh

function DisplayHelp()
{
    echo ""
    echo "NAME"
    echo "    set_rpfilter.sh - Set the reverse path filter  "
    echo ""
    echo "SYNOPSIS"
    echo "    bash set_rpfilter.sh [ --mode=0|1|2 ] [ --help ]"
    echo ""
    echo "DESCRIPTION"
    echo "    Set the reverse path filter mode"
    echo "    Used to allows the eBUS SDK to find the GEV device that"
    echo "    are on a different subnet"
    echo "    For more information, read the RHEL knowledge note:"
    echo "    https://access.redhat.com/knowledge/solutions/53031"
    echo "    This script can only used by root or sudoer"
    echo "    --mode=0|1|2       Set the mode from the command line"
    echo "                          0 - No source validation ( recommended )"
    echo "                          1 - RFC3704 Strict Reverse Path"
    echo "                          2 - RFC3704 Loose Reverse Path"
    echo "    --restartnetworkstack=yes|no"
    echo "                       Bypass the question and apply this answer about"
    echo "                        the need to reboot the network stack"
    echo "    --help             Display this help"
    echo ""
    echo "COPYRIGHT"
    echo "    __RESSOURCE_VERSION_COMPANY__ __RESSOURCE_VERSION_COPYRIGHT__"
    echo ""
    echo "VERSION"
    echo "    __RESSOURCE_VERSION_NUMBER__"
    echo ""
}

# Parse the input arguments
for i in $*
do
    case $i in        
        --mode=*)
            MODE=${i#*=} 
            ;;  
        --restartnetworkstack=*)
            RESTARTNETWORKSTACK=${i#*=} 
            ;;          
        --help)
            DisplayHelp
            exit 0
        ;;
        *)
        # unknown option
        DisplayHelp
        exit 1
        ;;
    esac
done

# Default variables
USER_ID=`id -u`

# Check required priviledge
if [ "$USER_ID" != "0" ]; then
  echo "Can only run this script as super user (root)."
  exit 1
fi

  echo "Configuration of the rp_filter."
  echo ""
  echo "For more information, read the RHEL knowledge note:"
  echo " https://access.redhat.com/knowledge/solutions/53031"
  echo ""

# ask for the unknown if not specified on the command line
# or the command line miss some operation

if [ -z "$MODE" ]; then
    ASK="yes"
elif [ "$MODE" = "2" ]; then
    ASK="no"
elif [ "$MODE" = "1" ]; then
    ASK="no"
elif [ "$MODE" = "0" ]; then
    ASK="no"
else
    echo "This mode is unknown"
    echo ""
    ASK="yes"
fi
if [ "$ASK" = "yes" ]; then
  echo ""
  echo "Supported modes:"
  echo " 0 - No source validation"
  echo " 1 - RFC3704 Strict Reverse Path"
  echo " 2 - RFC3704 Loose Reverse Path ( recommended )"
  echo ""
  ANSWER="not set"
  until [ "$ANSWER" = "0" -o "$ANSWER" = "1" -o "$ANSWER" = "2" ]; do
    echo -n "Which mode do you want [0|1|2]? Default is 0. "
    read ANSWER
    if [ -z "$ANSWER" ]; then
      ANSWER="0"
    fi
  done
  MODE=$ANSWER
fi

if [ -z "$RESTARTNETWORKSTACK" ]; then
    ASK="yes"
elif [ "$RESTARTNETWORKSTACK" = "yes" ]; then
    ASK="no"
elif [ "$RESTARTNETWORKSTACK" = "no" ]; then
    ASK="no"
else
    echo ""
    echo "The entry for --restartnetworkstack is unknown."
    echo ""
    ASK="yes"
fi
if [ "$ASK" = "yes" ]; then
  ANSWER="not set"
  until [ "$ANSWER" = "yes" -o "$ANSWER" = "no" ]; do
    echo -n "Do you want to restart the network stack to apply the changes [yes]? "
    read ANSWER
    if [ -z "$ANSWER" ]; then
      ANSWER="yes"
    fi
  done
  RESTARTNETWORKSTACK=$ANSWER
fi

# Make the validation of the value provided
if [ $MODE = "2" ]; then
  echo "Set mode Loose Reverse Path"
elif [ $MODE = "1" ]; then
  echo "Set mode Strict Reverse Path"
elif [ $MODE = "0" ]; then
  echo "Set mode No source validation"
fi
echo "" 

if [ "$RESTARTNETWORKSTACK" = "yes" ]; then
  echo "The network stack will be restarted"
  echo ""
elif [ "$RESTARTNETWORKSTACK" = "no" ]; then
  echo "The network stack will not be restarted. Consider restarting it if any problems."
fi

# Now perform the operations
for i in /proc/sys/net/ipv4/conf/*/rp_filter; do echo $MODE > $i; done

# If net.ipv4.conf.default.rp_filter present in /etc/sysctl.conf, change value to "$MODE"
#	else, add "net.ipv4.conf.default.rp_filter = $MODE" to /etc/sysctl.conf
if grep --silent ^net.ipv4.conf.default.rp_filter /etc/sysctl.conf; then
    sed -i 's/^net.ipv4.conf.default.rp_filter.*/net.ipv4.conf.default.rp_filter = '$MODE'/g' /etc/sysctl.conf
else
    echo "" >> /etc/sysctl.conf
    echo "net.ipv4.conf.default.rp_filter = $MODE" >> /etc/sysctl.conf
fi

if [ "$RESTARTNETWORKSTACK" = "yes" ]; then
  service network restart
fi

echo ""

