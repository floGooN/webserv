from cgi import FieldStorage



form = FieldStorage()
objectif = form.getvalue('objectif', '')
design = form.getvalue('design', '')
rdv = form.getvalue('rdv', '')
delai = form.getvalue('delai', '')
maintenance = form.getvalue('maintenance', '')
seo = form.getvalue('SEO', '')

final_value = 0
if design == 'oui':
    final_value += 200
else:
    final_value += 100
if rdv == 'oui':
    final_value += 200
if delai == '1 mois':
    final_value += 300
else:
    final_value += 200
if maintenance == 'oui':
    final_value += 100
if seo == 'oui':
    final_value += 299

body = f"""
<html>
 <body>
 <p>Le prix de votre devis est de: <strong>{final_value}</strong>.</p>
 </body>
</html>
"""


print(f"Content-type: text/html\n")
print(f"Status: 200 OK\n")
print(f"HTTP/1.1\n")
print(f"Content-Length: {len(body)}\n\n")


print(body)