#!/usr/bin/perl

use strict;
use warnings;
use CGI;


my $query = CGI->new;

my $upload_dir = "../../../uploads";
my $filename = $query->param('fileToUpload');
my $upload_filehandle = $query->upload('fileToUpload');

if (!$filename) 
{
    print "<html><body><p>No file uploaded.</p></body></html>";
    exit;
}
my $full_path = "$upload_dir/$filename";
open (my $upload_file, ">$full_path") or die "Cannot open file: $!";
binmode $upload_file;

while (my $ligne = <$upload_filehandle>) {
    print $upload_file $ligne;
}

close $upload_file;
close $upload_filehandle;



my $body = " <header>
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
<p>Thanks for uploading your Document!</p>
</body>
</html>";

print $body;