#!/usr/bin/python3

import cgi
import cgitb
cgitb.enable()

form = cgi.FieldStorage()


objectif = form.getvalue('objectif', '')
design = form.getvalue('design', '')
rdv = form.getvalue('rdv', '')
delai = form.getvalue('delai', '')
maintenance = form.getvalue('maintenance', '')
seo = form.getvalue('SEO', '')


finalValue = 0
if design == "oui":
    finalValue += 200
else:
    finalValue += 100

if rdv == "oui":
    finalValue += 200

if delai == "1 mois":
    finalValue += 300
elif delai == "2 mois":
    finalValue += 200

if maintenance == "oui":
    finalValue += 100

if seo == "oui":
    finalValue += 299



print("<html>")
print("<body>")
print("<p>Le prix de votre devis est de: <strong>{}</strong>.</p>".format(finalValue))
print("</body>")
print("</html>")

