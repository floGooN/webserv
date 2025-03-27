#!/usr/bin/perl

use strict;
use warnings;
use CGI;


my $cgi = CGI->new;

my $objectif = $cgi->param('objectif') || "";
my $design = $cgi->param('design') || "";
my $rdv = $cgi->param('rdv') || "";
my $delai = $cgi->param('delai') || "";
my $maintenance = $cgi->param('maintenance') || "";
my $seo = $cgi->param('SEO') || "";

my $finalValue = 0;

$finalValue += ($design eq "oui" ? 200 : 100);
$finalValue += ($rdv eq "oui" ? 200 : 0);
$finalValue += ($delai eq "1 mois" ? 300 : 200);
$finalValue += ($maintenance eq "oui" ? 100 : 0);
$finalValue += ($seo eq "oui" ? 299 : 0);

my $style = sprintf "<header>\n <style>         body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            font-family: 'Poppins', sans-serif;
            font-weight: 300;
            font-size: 15px;
            line-height: 1.7;
            color: #c4c3ca;
            background-color: #1f2029;
            overflow-x: hidden;
            margin: 0;
        } </style> </header>";
my $body = sprintf "<html>\n <body>\n <p>Le prix de votre devis est de: <strong>%d</strong>.</p>\n </body> \n </html>", $finalValue;

print $style;
print $body;