export VERBOSE="/bin/echo -ne" ;

unset arrayBuildProperty;
unset BPArrayName BPFileConf BPApp ACTION KEY VALUE REPLACEBY BPExtension ;

function arrayBuildProperty()
{
  local StrArrayName=${BPArrayName:=BuildProperty} ;
  local StrFileSave=${BPFileConf:=.${StrArrayName}} ;
  local StrAppArduino=${BPApp:=arduino-cli} ;
  local IntDecl=$( declare -A | grep -c "${StrArrayName}" ) ;
  local StrAction=${ACTION:=view};
  local StrKey=${KEY:=none} ;
  local StrValue=${VALUE:=''} ; 
  local StrRelaceBy=${REPLACEBY:=''};
  local StrExtension=${BPExtension:=parse} ;
  local BoolOverwriteExt=${BPIsOvExt:=False} ;
  local IsKeyFound=False;
  local IsArrayDecl=True
  local VERBOSE='echo -ne ' ;
  function __GetKey()
  {
    local IsFound=False;
    eval "local -a ArrayItem=( \${!${GKArrayName}[@]} )";
    for item in ${ArrayItem[@]} ; do  
      if [ "${item}" == "${GKKey}" ] ; then 
	IsFound=True;
      fi
    done ;
    echo ${IsFound}
  }
  
  if [ ${IntDecl:=0} -lt 1  ] ; then 
    ### Apparently array is not existing inside declare
    ${VERBOSE} "Array ${StrArrayName} not declared, trying to declare it.\nBecause ${StrArrayName} it's not declared we can't do action such VIEW/CHANGE/DEL/SAVE.\nTry to use LOAD if the filename ${StrFileSave} exist.\n\n" > /dev/stderr
    IsArrayDecl="False" ;
    declare -A ${StrArrayName} ; 
  fi 
  case "${StrAction^^}" in  
    VIEW)
      ${VERBOSE} "View ${StrArrayName} information.\n" > /dev/stderr ;
      eval "local -a ArrayItem=( \${!${StrArrayName}[@]} )"
      for item in ${ArrayItem[@]} ; do
        eval "StrValue=\${${StrArrayName}[${item}]}"
        ${VERBOSE} "Key ${item}\tValue:${StrValue}\n" > /dev/stderr
      done 
    ;;
    ADD)
      ${VERBOSE} "Add inside ${StrArrayName} a key and value for arduino-cli compile directive.\n" > /dev/stderr ;
      ${VERBOSE} "Adding Key:\"${StrKey}\"\nValue: \"${StrValue}\" to ${StrArrayName}\n" > /dev/stderr ;
      IsKeyFound=$( GKArrayName=${StrArrayName} GKKey=${StrKey} __GetKey ) ;
      if [ "${IsKeyFound:=False}" == "True" ] ; then 
        ${VERBOSE} "Adding element from [VALUE] inside [KEY].\n"
	eval """StrContent=\${${StrArrayName}[\"${StrKey}\"]};"""     
	eval """${StrArrayName}[\"${StrKey}\"]=\"${StrContent} ${StrValue}\""""
      else
	${VERBOSE} "Adding a new [KEY] with element from [VALUE].\n"
	eval """${StrArrayName}[\"${StrKey}\"]=\"${StrValue}\"""" ;
      fi
    ;;
    CHANGE)
      ${VERBOSE} "Change a ${StrArrayName} key and value for arduino-cli compile directive.\n" > /dev/stderr ;
      IsKeyFound=$( GKArrayName=${StrArrayName} GKKey=${StrKey} __GetKey ) ;
      if [ "${IsKeyFound:=False}" == "True" ] ; then 
        ### With StrRelaceBy REPLACEBY
	if [ "${StrRelaceBy}" == '' ] ; then 
	  ${VERBOSE} "Key Found: ${StrKey}\nChanging content.\n" > /dev/stderr ;
	  eval """${StrArrayName}[\"${StrKey}\"]=\"${StrValue}\";"""
	else
	  ${VERBOSE} "REPLACEBY specified, will get the VALUE and REPLACE it by REPLACEBY.\nKey Found: ${StrKey}\nChanging content [${StrValue}] by [${StrRelaceBy}].\n";
	  eval """StrRepl=\${${StrArrayName}[\"${StrKey}\"]};"""
	  StrRepl=${StrRepl//${StrValue}/${StrRelaceBy}}
	  eval """${StrArrayName}[\"${StrKey}\"]=\"${StrRepl}\";"""
	fi
      else
        ${VERBOSE} "Warning key ${StrKey} does not exist.\n" > /dev/stderr
      fi 
    ;;
    DEL)
      ${VERBOSE} "Delete a BuildProperty key.\n" > /dev/stderr;
      IsKeyFound=$( GKArrayName=${StrArrayName} GKKey=${StrKey} __GetKey ) ;
      if [ "${IsKeyFound:=False}" == "True" ] ; then
	if [ "${StrValue}" != "" ] ; then 
	  ${VERBOSE} "Deleting element [VALUE] from [KEY].\n"
	  eval """StrContent=\${${StrArrayName}[\"${StrKey}\"]};""" 
	  StrContent=${StrContent//${StrValue}/} ;    
	  eval """${StrArrayName}[\"${StrKey}\"]=\"${StrContent}\""""
	else
	  ${VERBOSE} "Deleting [KEY].\n"
	  ${VERBOSE} "Key Found: ${StrKey}\n" > /dev/stderr;
	  eval "unset ${StrArrayName}[${StrKey}]" ;
	fi
      else
        ${VERBOSE} "Warning key ${StrKey} does not exist, it can't be deleted." > /dev/stderr
      fi
    ;;
    SHOW)
      ${VERBOSE} "Show key and value from arduino-cli command.\nMight be good if you overwrite something you can compare, what you overwrite during an arduino-cli compile command.\n" > /dev/stderr
      IsKeyFound=$( GKArrayName=${StrArrayName} GKKey=${StrKey} __GetKey ) ;
      if [ "${IsKeyFound:=False}" == "True" ] ; then 
        ${VERBOSE} "\nKey Found: ${StrKey} inside your ${StrArrayName}\n" > /dev/stderr ;
	if [[ "${StrKey}" =~ ^[a-zA-Z]*. ]] ; then 
	  ${StrAppArduino} compile --show-properties | grep "^${StrKey}" 2> /dev/null ;
	else
	  ${VERBOSE} "\nKey [${StrKey}] is not an Arduino compiler directive or show-properties value.\n" > /dev/stderr ;
	fi ; 
      else
        ${VERBOSE} "\nWarning key '${StrKey}' is not inside your ${StrArrayName}.\nGrep will be base on the request not the key.\n" > /dev/stderr
	${StrAppArduino} compile --show-properties | grep "^${StrKey}" 2> /dev/null ; 
      fi      
    ;;
    SAVE)
      ${VERBOSE} "Save all BuildProperty key/value into local-path file.File-name is:${StrFileSave}\n" > /dev/stderr ;
      echo "IyEvYmluL2Jhc2ggCg==" | base64 --decode > ${StrFileSave} ; 
      echo "declare -A ${StrArrayName}" >> ${StrFileSave} ;
      eval "local -a ArrayItem=( \${!${StrArrayName}[@]} )"
      for item in ${ArrayItem[@]} ; do  
        eval  """echo \"${StrArrayName}[\"\"${item}\"\"]=\"\${${StrArrayName}[\"${item}\"]}\"\"""" | sed 's/\[/\[\"/g;s/\]/\"\]/g;s/\]=/\]="/g;s/$/\"/' >> ${StrFileSave} ;
      done ;
      chmod 775 ${StrFileSave} ; 
    ;;
    LOAD)
      ${VERBOSE} "Restore all BuildProperty key/value into local-path file.File-name is:${StrFileSave}\n" > /dev/stderr ;
      if [ -e ${StrFileSave} ] ; then 
        ${VERBOSE} "Reading the file ${StrFileSave}\n" > /dev/stderr ;
        grep -v "/bin/bash" ${StrFileSave} ; 
      else
        ${VERBOSE} "Filename ${StrFileSave} is not present to restore all the key for ${StrArrayName} or not in local current path.\n" > /dev/stderr;
      fi
    ;;
    PARSE)
      ${VERBOSE} "Parse a file-name and apply all __TAG__ to value and write the filename.\nYou must add an index name 'FILENAME', to allow Parsing action\n\n" > /dev/stderr ;
      eval "StrFileName=\${${StrArrayName}[FILENAME]}" ;
      if [ "${BoolOverwriteExt}" == "True" ] ; then 
        StrFileNameFinal="${StrFileName%%.*}.${StrExtension}"
        ${VERBOSE} "Extension is set to overwrite the initial extension name of file name\nand leave a final extension to: ${StrFileNameFinal}\n\n"
        if [ -f ${StrFileNameFinal} ] ; then 
          ${VERBOSE} "Existing filename ${StrFileNameFinal}, will be rename into ${StrFileNameFinal}.orig\n" ;
          mv ${StrFileNameFinal} ${StrFileNameFinal}.orig ;
        fi 
      else
        StrFileNameFinal="${StrFileName}.${StrExtension}"
        ${VERBOSE} "Extension name is set to add extension name at the file name\nand leave a final extension to: ${StrFileNameFinal}\n\n"
      fi 
      IsFileName="False" ; 
      eval "local -a ArrayItemName=( \${!${StrArrayName}[@]} )"
      for itemName in ${ArrayItemName[@]} ; do 
        if [ "${itemName}" == "FILENAME" ] ; then 
          IsFileName="True" ; 
        fi 
      done 
      if [ "${IsFileName}" == "True" ] ; then
        ${VERBOSE} "FILENAME index found file name to parse: ${StrFileName}\nFilename to be write ${StrFileNameFinal}\n\n" ; 
        eval "local -a ArrayItem=( \${!${StrArrayName}[@]} )"
        if [ -f ${StrFileName} ] ; then 
          cp -f ${StrFileName} ${StrFileNameFinal} ;
        fi
        for item in ${ArrayItem[@]} ; do  
          eval  """StrValue=\${${StrArrayName}[\"\"${item}\"\"]}""" ;
          if [[ "${item}" =~ ^__ ]] ; then 
           ${VERBOSE} "Processing tag:[${item}], value:[${StrValue}]\n" ;  
           eval """sed s/${item}/${StrValue}/g < ${StrFileNameFinal} > ${StrFileNameFinal}.1""" ; 
           if [ -f ${StrFileNameFinal}.1 ] ; then 
            mv -f ${StrFileNameFinal}.1 ${StrFileNameFinal} ;
           fi 
          else
           ${VERBOSE} "Key: ${item} will not be used as __TAG__.\n"
          fi 
        done ;
        if [ -f ${StrFileNameFinal}.1 ] ; then 
         rm -f ${StrFileNameFinal}.1 ;
        fi 
      else
        ${VERBOSE} "FILENAME index not found. use the ADD command to add the FILENAME\nindex with filename to parse.\n"
      fi 
    ;;
    
    HELP|*)
      ${VERBOSE} "H4sIAAAAAAACA4VWTW/bSAy961fwsEBaYKOizS2AD2qiNsZ6baP2pi1QIBhLdDSNPOPOjPzx75fkSJacBK0PQRTxPZKPj3SWlfZwh/U2TZKxgcCPFT2CDuAru4eN+mkdqCJoa0DVtd1jCasjNF6bR1g7u4Et/diGZD5bjL9d3iun1apGyG6W49kUlCnhn/w7xRMuWNiiW1u36RiZ1HtbaBX0DhPlnDqCURsK/tjoupw7S4hwhH2liwqoOiHSxusSYd2YtjBXNtrYy4Iq0TUmlAhNpUyBHO/BrvuQWgOFbbgwbaBQHvl1RKpY1BqyGJ0snjAUFXqSJ9spCuDWOnionG0eq7bV6yS5yydz4M+y0zFN7sf5V/nbRPvACsITHkWWnaobZE1WUiWUjWNNB5WkSXZ7K2DIyhIUGNwLvLAigCMpWpQ662BLoqYJdB8Cx5gnYyNBaUkVYwOgYwFCxV2ZgCb8DXoN99nkv7zH6zaWxnzkB1WWlFkFwSG1Qorxr8zMw0uT23wSobdYY0BOzV1HRNAccmKXEK6PZkzqVmrHD1IC7HWoKLdpc0fJPCpXVAOG1g5dCSyuw43dnTX2PCoWenOXTT/nxHFDdnmMb2Pq2XTyXYzD0WmyuJvFOS6qVkMx/9BWYqlw4WOZBMnu8whROxn0mvvjDOfeJrJ3LWQyy+LAv6AP1j2PlJSe2MohaJ59WUiiuXKetZY81C5heE6eg0efxpN8mv2bt/KExtHe9BpuGVsKNIVc0bIF9Ug+4Xl0Lj1RRil7MPeUye4y+aYhp0cY/V3W/eFhmX1+eJDXKurTo+nQNHXJCaKxCCHlkA1SSAY29ryxV93toJoKhyqcOg54CGk4hOiaLuVrtUqXA2Y6fS2paTYrdPAejrYhQlrXjmev6WGFtGYIF3Qya3vxGsEHQVIjkugT18VO64vrbdjD5e6djfqM+gp8wK0Q+6Bc3KJWIRjsQX64fp+252jEp4Mu76irX1w9ksJ/m48+H16QnMwTWU7N/IHo6kQUPfq78NlW9OPhz2Nr1/3Ly+6GfZznB9pmT6Ej7H6Ls2nNQCq5oZuhMXQpB/Nva09P8FfTjP1sR6lGS0cXh06J2zsd2oPSOMcHpc/PnOTHZ0n69yl8ldumzABEh1SmKad5eEtfelX2Jo5evK1oMQMZeGqDLvCavnAsyOnovprYKrS7Dn812sn6FvzVw6+Qt0/JYeCnAxYNC//DJPwGfvz1BrqhCeXLmcFbJtyoJ+R/E2qrWLI0mbmYpPO92D6F9J0OHus1GfV/QBffSm4IAAA=" | base64 --wrap=0 -d | gzip -dc ;
    ;;
  esac 
}

unset arduino-compile;
function arduino-compile()
{
  local IntDecl=$( declare | grep -c "BuildProperty" ) ;
  local StrPathBuild=${ACBuildPath:=build} ;
  local IsAssociative="False";
  local StrCmdTpl="arduino-cli compile --verbose __SWITCHES__ __BUILDPROPERTY__ __PARALLEL_JOBS__ __BUILD__" ; 
  local BoolDebug=${IsDebug:=False} ; 
  local IntJobP=${ACJobs:=12} ; 
  local IsBuildPath=False;
  local StrCmd ;
  local -a ArrayIndex ; 
  local VERBOSE='echo -ne' ; 
  if [ ${IntDecl:=0} -gt 0 ] ; then 
    ### There is a BuildProperty Array.
    ArrayIndex=( ${!BuildProperty[@]} ) ; 
    if [[ ${ArrayIndex[0]} =~ ^[a-zA-Z0-9]* ]] ; then 
      ### It's an associative array and can be used to pad __BUILDPROPERTY__ instruction.
      IsAssociative="True";
    else
      ### It's not associative array, exclude uses of BuildProperty
      IsAssociative="False";
      ${VERBOSE} "No Associative array BuildProperty.\nPerhaps you can do:\ndeclare -a BuildProperty and adding among\nvariable-index compiler.c.extra_flags and/or\ncompiler.c.extra_flags and fill them with\ncompiler directive for your compilation.\n";
    fi
  else
    ${VERBOSE} "No BuildProperty array declare and it's not an associative array. Declare it and you can use index-name command 'from arduino-cli compile --show-properties' to fill index-key-name and add your own compiler directive such include, define...\n" ;
  fi
  
  #test -d ${StrPathBuild} && rm -rf ${StrPathBuild}/* ; 
  
  if [ "${IsAssociative}" == "False" ] ; then 
    StrCmd=${StrCmdTpl//__BUILDPROPERTY__/};
    StrCmd=${StrCmd//__PARALLEL_JOBS__/--jobs ${IntJobP}}
    #StrCmd=${StrCmd//__BUILD__/--output-dir ${StrPathBuild}} ;
    if [ "${BoolDebug}" == "True" ] ; then 
      ${VERBOSE} "DEBUG: Cmd:[${StrCmd}]\n"
    else
      eval "${StrCmd}" ;
    fi
  else
    StrCmd=${StrCmdTpl//__PARALLEL_JOBS__/--jobs ${IntJobP}}
    #StrCmd=${StrCmd//__BUILD__/--output-dir ${StrPathBuild}} ;
    for item in ${!BuildProperty[@]} ; do 
      if [[ ${item} =~ ^-- ]] ; then
        ### Adding a switch content.
	if [ "${BuildProperty[${item}]}" == "" ] ; then 
	  ### Alone switches like --export-binaries --preprocess --quiet --upload
	  StrCmd="""${StrCmd//__SWITCHES__/${item} __SWITCHES__}"""
	else
	  ### adding switches with value.
	  StrCmd="""${StrCmd//__SWITCHES__/${item}=\"${BuildProperty[${item}]}\" __SWITCHES__}"""
	fi 
      else
	### Adding a Build property. 
	StrCmd="""${StrCmd//__BUILDPROPERTY__/--build-property ${item}=\"${BuildProperty[${item}]}\" __BUILDPROPERTY__}"""
      fi
      if [[ ${item} =~ ^--output-dir ]] ; then 
        ### Detected --output-dir inside the Array content.
	StrCmd=${StrCmd//__BUILD__/} ;
	IsBuildPath=True;
      fi 
    done
    if [ "${IsBuildPath}" == "False" ] ; then
      ### There is no --output-dir inside the Array content and se the default one.
      StrCmd=${StrCmd//__BUILD__/--output-dir ${StrPathBuild}} ;
    fi 
    StrCmd=${StrCmd//__BUILDPROPERTY__/} ; 
    StrCmd=${StrCmd//__PARALLEL_JOBS__/} ; 
    StrCmd=${StrCmd//__SWITCHES__/} ;
    if [ "${BoolDebug}" == "True" ] ; then 
      ${VERBOSE} "DEBUG: Cmd:[${StrCmd}]\n"
    else
      eval "${StrCmd}" ;
    fi 
  fi 
}
