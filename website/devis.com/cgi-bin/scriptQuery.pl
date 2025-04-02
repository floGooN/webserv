#!/usr/bin/perl

use strict;
use warnings;
use CGI;


my $cgi = CGI->new;

my $query = $ENV{'QUERY_STRING'};
my $alive = $ENV{'KEEP_ALIVE'};
my $value1;
my $value2;

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
            $value2 = $val;
        }
    }
}

my $body = "
<header>
<style>       
body {
    display: flex;
    justify-content: center;
    flex-direction: column;
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
    }
</style>
</header>
<html>


<body>


<p>Your email address: $value1\n</p>
<p>Your adress: $value2</p>

</body>
</html>";



print $body;

