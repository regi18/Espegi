Dim o
Set o = CreateObject("MSXML2.XMLHTTP")
o.open "GET", "http://<IP Address>/togglecmd", False
o.send
