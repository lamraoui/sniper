<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
 <HEAD>
   <title>SNIPER Alpha Online</title>
   <LINK rel="stylesheet" type="text/css" href="style.css">
   <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
   <link rel="icon" type="image/png" href="logo2.png" />
 </HEAD>
<BODY>

<div id="header">
<h1> SNIPER Alpha Online </h1>
</div>

<FORM method="POST" action="index.php" id="formCGI">
<div id="main">
 
	<div id="main.header">
	<p> This is a web application for running the alpha version of SNIPER, a tool for automatically localizing errors in imperative programs.</p><br>
	
	</div>
 
   	<div id="code">
	Type your C source code in below:
	<input type="hidden" name="input" id="code-source" value="test">	
	<div id="editor"><?php
	if(isset($_POST['run'])){
		echo $_POST['input'];	
	} else {
		echo "\nvoid foo(int x) {\n\n";
		echo "\tint y;\n";
                echo "\tif(x>0) {\n";
		echo "\t\ty = 1;\n";
		echo "\t} else {\n";
		echo "\t\ty = 0;\n";
		echo "\t}\n\n";
		echo "\tassert(y>0);\n";
                echo "}\n";
	} ?></div>
   	</div>
   
   	<div id="output">
	Output:<br>
	<TEXTAREA disabled name="output"><?php	
	// Options
	$arg = "";
	if(isset($_POST['printTF']) && $_POST['printTF']=='Yes') {
		$arg = $arg . " -print-tf";
	}
	if(isset($_POST['printIR']) && $_POST['printIR']=='Yes') {
		$arg = $arg . " -print-ir";
	}
	if(isset($_POST['printDura']) && $_POST['printDura']=='Yes') { 
               $arg = $arg . " -print-dura";
        }
	if(isset($_POST['debugMode']) && $_POST['debugMode']=='Yes') {
                $arg = $arg . " -dbg-msg";
        }
	if(isset($_POST['checkMethod'])) {
    		if($_POST['checkMethod']=='pe') {
			$arg = $arg." -pe";
		} else if($_POST['checkMethod']=='concolic') {
			$arg = $arg." -ce";
		}
	}
	if(isset($_POST['unrollCount'])) {
		$count = $_POST['unrollCount'];
		if(is_numeric($count) && $count>0) { 
	       		$arg = $arg." -unroll-count ".$_POST['unrollCount'];
        	}
	}
	if(isset($_POST['run'])){
		$code = $_POST['input'];
		// $code = escapeshellarg($code); 
		$clientIP = $_SERVER['REMOTE_ADDR'];
		$sourceFile = "/tmp/source_".$clientIP.".c";
		$fh = fopen($sourceFile, 'w');
		if($fh==false) {
			print_r(error_get_last());	
		} else {
			fwrite($fh, $code);
			fclose($fh);
			system("/usr/local/bin/clang ".$sourceFile." -S -emit-llvm -O0 -g -o ".$sourceFile.".bc > ".$sourceFile."_error 2>&1"  );
			$errf = fopen($sourceFile."_error", 'r');
			// If the compilation failed		
			if(!file_exists($sourceFile.".bc")) {
				echo "ERROR:\n\n";
				$errf = fopen($sourceFile."_error", 'r');
                        	if($errf) {
                                	while(!feof($errf)) {
                                        	$line = fgets($errf,255);
                                        	echo $line;
                                	}
                                	fclose($errf);
                        	}

			}
			passthru("/var/www/html/private/tool -function foo ".$arg." ".$sourceFile.".bc");
		}
	} 
	?></TEXTAREA>
   	</div>
   	
   	<div id="option">
	<fieldset>
	<input type="checkbox" name="printTF" value="Yes">Print trace formula
	<input type="checkbox" name="printIR" value="Yes">Print IR
	<input type="checkbox" name="printDura" value="Yes">Print duration
	<input type="checkbox" name="debugMode" value="Yes">Debug mode
	<BR>
	<input type="radio" name="checkMethod" value="bmc" checked>BMC
	<input type="radio" name="checkMethod" value="pe">Path Exploration
	<input type="radio" name="checkMethod" value="condolic" disabled><font color="#848484">Concolic Execution</font>
	<BR>
	Loops unroll count <input type="number" name="unrollCount" value="10" step="1"  min="0">
	<BR>
	<BR>
	<INPUT type="submit" value="Check & Localize" name="run" onclick="getValue();">
	</fieldset>
	</div>
</div>
</FORM>

<script src="src-min/ace.js" type="text/javascript" charset="utf-8"></script>
<script>
     var editor = ace.edit("editor");
     editor.setTheme("ace/theme/idle_fingers");
     editor.getSession().setMode("ace/mode/c_cpp");
</script>
<script type="text/JavaScript">
   function getValue() {
       document.getElementById('code-source').value = editor.getSession().getValue();
   }
 </script>
  
</BODY>
</HTML>