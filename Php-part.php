<?php
   if( $_REQUEST["endthis"])
   {
       //clear doc
       file_put_contents('dataDisplayer.html', '');
       
   }
   else if( $_REQUEST["var"]) 
   {
        //print var
        echo $_REQUEST['var'];
   }

$var1 = $_REQUEST['var'];

$WriteMyRequest=$var1;

file_put_contents('dataDisplayer.html', $WriteMyRequest, FILE_APPEND);

?>
