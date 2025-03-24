#!/usr/bin/perl

use strict;
use warnings;
use CGI;


my $cgi = CGI->new;

my $query = $ENV{'QUERY_STRING'};

if ($query)
{
    my @pairs = split(/&/, $query);
    foreach my $pair (@pairs)
    {
        my ($key, $val) = split(/=/, $pair);
        if ($key eq "email")
        {
            $value1 = $val;
        }
        if ($key eq "adresse")
        {
            my $value2 = $val;
        }
    }
}

my $body = "<HTML>
<HEAD>
<TITLE>Info Client</TITLE>
</HEAD>

<BODY>


<P>Your email address: $value1</P>
<P>Your adress : $value2</P>
</BODY>
</HTML>";

print $cgi->header(-type => 'text/html', -status => '200 OK', -http11 => 'HTTP/1.1', -length => length($body));
print $body;