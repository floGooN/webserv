#!/usr/bin/python3

import cgi # type: ignore



form = cgi.FieldStorage()


objectif = form.getvalue('objectif')
design = form.getvalue('design')
rdv = form.getvalue('rdv')
delai = form.getvalue('delai')
maintenance = form.getvalue('maintenance')
seo = form.getvalue('SEO')
style = ("<header><style> body {display: flex;justify-content: center;flex-direction: column;align-items: center;height: 100vh;font-family: 'Poppins', sans-serif;font-weight: 300;font-size: 15px;line-height: 1.7;color: #c4c3ca;background-color: #1f2029;overflow-x: hidden;margin: 0;}</style></header>")

if design and rdv and delai and maintenance and seo :
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


    body = ("<html><body><p>Le prix de votre devis est de: <strong>{}</strong>.</p></body></html>").format(finalValue)

    print(style)
    print(body)
else :
    noCompleteBody = "<html>\n <body>\n <p>Il nous manque des informations pour vous donnez un devis complet veuillez remplir tout les champs.</p>\n </body> \n </html>"
    print(style)
    print(noCompleteBody)
    





