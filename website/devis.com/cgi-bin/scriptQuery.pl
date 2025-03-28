#!/usr/bin/perl

use strict;
use warnings;
use CGI;


my $cgi = CGI->new;

my $query = $ENV{'QUERY_STRING'};

# if ($query)
# {
#     my @pairs = split(/&/, $query);
#     foreach my $pair (@pairs)
#     {
#         my ($key, $val) = split(/=/, $pair);
#         if ($key eq "email")
#         {
#             $value1 = $val;
#         }
#         if ($key eq "adresse")
#         {
#             my $value2 = $val;
#         }
#     }
# }

my $body = "<html>


<body>


<p>Your email address: $query</p>

</body>
</html>";


print $body;

# print my $body = sprintf "<html>\n <body>\n <p>Le prix de votre devis est de: <strong>%d</strong> euros.</p>\n </body> \n </html>", $finalValue;;