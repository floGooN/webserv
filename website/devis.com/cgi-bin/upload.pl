#!/usr/bin/perl

use strict;
use warnings;
use CGI;


my $query = CGI->new;

my $upload_dir = "./uploads";
my $filename = $query->param('fileToUpload');
my $upload_filehandle = $query->upload('fileToUpload');

if (!$filename) 
{
    print "<html><body><p>No file uploaded.</p></body></html>";
    exit;
}

open (my $upload_file, ">$filename") or die "Cannot open file: $!";
binmode $upload_file;

while (my $ligne = <$upload_filehandle>) {
    print $upload_file $ligne;
}

close $upload_file;
close $upload_filehandle;



my $body = "<html>
<body>
<p>Thanks for uploading your Document!</p>
</body>
</html>";

print $body;