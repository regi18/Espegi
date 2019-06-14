Dim o
Set o = CreateObject("MSXML2.XMLHTTP")
o.open "GET", "http://<IP Address>/temperaturecmd", False
o.send
x=msgbox(o.responseText ,0, "Indoor Temperature")
