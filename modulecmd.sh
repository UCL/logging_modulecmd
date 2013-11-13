#!/bin/bash 

#############################
# A fairly basic wrapper around modulecmd to allow actions on loading or unloading a module.
#
# Ian Kirker, 2013


# Params to edit
PathToModuleCmd=/shared/ucl/apps/modules-3.2.10-modlog
RealModuleCommand=$PathToModuleCmd/modulecmd
ExternalLogger=$PathToModuleCmd/mysql_injector


LogMode="external"
# LogMode="file"

# Obviously if you're logging to files, you'll want to 
#   put them somewhere you can gather/access them.
# This is our Lustre file system, for example.
# Only used if LogMode = "file"
LogFilesDirectory=/scratch/scratch/.modusage


# End of params
#############################

Date=`date +%s`

## Create log file name
# For Logging Messages in files rather than using an external tool

SessionPID=$1 # This is added by our altered module shell function
TimeCode=${Date:0:6} 
# ^--- This gives a 10000 window on things being considered the same session - 
#        to let us track the same session, but also allow for PID recycling
#      It's hardly perfect, but it at least does something.
shift

UsageFile=${LogFilesDirectory}/$SessionPID.$TimeCode.`whoami`.`hostname`.${JOB_ID:=undefined}.${TASK_ID:=undefined}
umask 0077 # Thus files should be 0600


## Run actual module command

$RealModuleCommand $*

ModuleCommandReturnValue=$?


## Skip out sharpish without doing anything if there aren't any arguments to the module command
if [[ -z "$2" ]]; then # The command source is the first argument at this point
  exit
fi


## Copy the original arguments so we can use 'shift' later without screwing everything up
declare -a OriginalArguments

# I'm a little worried about this with arguments with spaces in
i=0
for Argument; do
  OriginalArguments[$i]=$Argument 
  let i=$i+1
done


## Gather information

CurrentUser=`whoami`
CurrentHost=`hostname`
CommandSource=$1 #(almost certainly 'bash')
shift

# You could do also do something here like :
# CurrentlyLoadedModules="$LOADED_MODULES" 
# But we don't currently log that


# Ignore any command line options

while [[ "${1:0:1}" == "-" ]]; do
  if [[ "${1:0:1}" == "-" ]]; then
    FirstLetter=${1//-/}
    FirstLetter=${FirstLetter:0:1}
    
    if [[ "$FirstLetter" == "u" ]]; then
      # User level switch takes an option, so do an extra shift
      shift;
    fi
  fi
shift
done


## Command case functions

LogPrintMessage() {
Command=$1
Target=$2
  case $Command in
    "Load")
      ActionPhrase="using $CommandSource loaded module $Target"
      ;;
    "Unload")
      ActionPhrase="using $CommandSource unloaded module $Target"
      ;;
    *)
      ActionPhrase="used action: '$Command' with target: $Target"
      ;;
  esac

  echo "[$Date] User `whoami` $ActionPhrase" | tee -a $UsageFile >&2
}

Log() {
  Action="$1"
  ModulePath="$2"

  # Erk. I forgot about these when I created the table on our system, which 
  #   now only accepts numerical values for job and task id.
  if [[ "$JOB_ID" == "undefined" ]]; then
    JOB_ID=0
  fi
  TASK_ID=${SGE_TASK_ID:-undefined}
  if [[ "$TASK_ID" == "undefined" ]]; then
    TASK_ID=0
  fi

  if [[ "$LogMode" == "external" ]]; then
    echo -e "$CurrentUser\n$CurrentHost\n$SessionPID\n$JOB_ID\n$TASK_ID\n$Action\n$ModulePath\n" | $ExternalLogger

  elif [[ "$LogMode" == "file" ]]; then
    echo "[$Date]"\
         " user:$CurrentUser"\
         " host:$CurrentHost"\
         " pid:$SessionPID"\
         " jobid:$JOB_ID"\
         " taskid:$TASK_ID"\
         " command:$Action"\
         " target:$ModulePath" | tee -a $UsageFile >&2
  fi
}

Error() {
  Message=$1

  if [[ "$LogMode" == "file" ]]; then
    echo "[$Date] ERROR: $Message" | tee -a $UsageFile >&2
  else
    echo "ERROR: $Message" >&2
  fi
}


LoadCommand() {
  for ModuleSpecifier; do
    ActualModule=`GetModulePath $ModuleSpecifier`
    if [[ "$ActualModule" != "" ]]; then
      Log "Load" $ActualModule
    else
      Error "Attempt to determine loaded module failed: user `whoami` module $ModuleSpecifier" 
    fi
  done
}

UnloadCommand() {
  for ModuleSpecifier; do
    ActualModule=`GetLoadedModule $ModuleSpecifier`
    if [[ "$ActualModule" != "" ]]; then
      ActualModule=`GetModulePath $ActualModule`
      if [[ "$ActualModule" != "" ]]; then
        Log "Unload" $ActualModule
      else
        Error "Attempt to determine path of unloaded module failed: user `whoami` module $ModuleSpecifier"
      fi
    else
      Error "Attempt to determine module unloaded failed: user `whoami` module $ModuleSpecifier with loaded modules $LOADED_MODULES"
    fi
  done
}

SwitchCommand() {
  if [[ -z "$2" ]]; then
    if UnloadCommand ${1%%/*}; then
      LoadCommand $1
    fi
  else
    if UnloadCommand $1; then
      LoadCommand $2
    fi
  fi
}

GetModulePath() {
  ModuleSpecifier=$1
  Possibilities=""
  for Directory in ${MODULEPATH//:/ }; do
    Possibilities=`echo $Possibilities; find $Directory/$ModuleSpecifier -type f 2>/dev/null`
  done
  
  sed 's/ /\n/g' <<<$Possibilities | sort | tail -n 1
}

GetLoadedModule() {
  ModuleSpecifier=$1
  Possibilities=""
  
  ModuleSpecifier=/${ModuleSpecifier%/}
  
  for Module in ${LOADEDMODULES//:/ }; do
    ModuleNameForCutting=/$Module

    while [[ -n "$ModuleNameForCutting" ]]; do
      if [[ "$ModuleSpecifier" == "$ModuleNameForCutting" ]]; then
        Possibilities="$Module $Possibilities"
        ModuleNameForCutting=""
      else
        ModuleNameForCutting=${ModuleNameForCutting%/*}
      fi
    done
  done
  
  sed 's/ /\n/g' <<<$Possibilities | sort | tail -n 1
}

if [[ "$ModuleCommandReturnValue" == "0" ]]; then
  case $1 in
    add|load)
      shift
      LoadCommand $*
      ;;
    unload|rm|remove)
      shift
      UnloadCommand $*
      ;;
    switch|swap)
      shift
      SwitchCommand $*
      ;;
    *)
      ;;
  esac
fi

exit $ModuleCommandReturnValue


