import os
from urllib.parse import parse_qs

query_string = os.environ.get('QUERY_STRING')
params = parse_qs(query_string)

param1 = params.get('email', [None])[0]
param2 = params.get('adresse', [None])[0]

body = """<HTML>
<HEAD>
<TITLE>Info Client</TITLE>
</HEAD>

<BODY>

<P>Thanks for uploading your photo!</P>
<P>Your email address: {param1}</P>
<P>Your adress : {param2}</P>
</BODY>
</HTML>"""



print(body)