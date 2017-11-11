// cscript %systemroot%/runadm.js "%1"
// cscript %systemroot%/runadm.js notepad "%1"
var c=WScript.Arguments.Count();
//WScript.StdOut.Write('count:'+c+'\n');
if(c<1){
	WScript.StdOut.Write('no command specified.');
}else{
	var command = WScript.Arguments.Item(0);
	var argument = "";
	for (var i = 1; i < c; ++i){
		argument += '"'+WScript.Arguments.Item(i) + '" ';
	}
	var sh=new ActiveXObject('shell.application');
	sh.ShellExecute(command, argument, '', 'runas', 1);
}
//var a=WScript.StdIn.ReadLine();

