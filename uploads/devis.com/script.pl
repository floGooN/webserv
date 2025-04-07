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
my $body =  "<html>\n <body>\n <p>Le prix de votre devis est de: <strong>$finalValue</strong>.</p>\n </body> \n </html>";


print $cgi->header(-type => 'text/html', -status => '200 OK', -http11 => 'HTTP/1.1', -length => length($body));
print $body;