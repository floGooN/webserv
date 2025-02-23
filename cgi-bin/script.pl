#!/usr/bin/perl
use CGI qw(:standard);
use POSIX 'strftime';
print "HTTP/1.1 200 OK\r\n";
print "Content-Type: text/html\n\n";
print "<HTML><HEAD><TITLE>Time</TITLE></HEAD><BODY>\n";

my $cgi = CGI->new;

my $value_final = 200;
my $pref_design = $cgi->param('design');
my $rdv = $cgi->param('rdv');
my $delai = $cgi->param('delai');
my $maintenance = $cgi->param('maintenance');
my $SEO = $cgi->param('SEO');

$value_final += ($pref_design eq 'oui') ? 200 : 100;
$value_final += ($rdv eq 'oui') ? 200 : 0; 
$value_final += ($delai eq '1 mois') ? 400 : 200;
$value_final += ($maintenance eq 'oui') ? 200 : 0;
$value_final += ($SEO eq 'oui') ? 299 : 0;

print "<h1>Le Calcul de votre Devis revient a : </h1>\n"
# print "<h2>$value_final</h2>\n"

# print "</BODY></HTML>\n";
