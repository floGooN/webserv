#!/usr/bin/perl

use strict;
use warnings;
use CGI;
use File::Basename;


#a voir si je dois rajouter des element dans la requete quon renvoi au server
# car la je renvoi seulement content-type + les balises html

my $cgi = CGI->new;
print $cgi->header('text/html');
my $filehandle = $cgi->upload('fileToUpload');
if (defined $filehandle) 
{
    my $filename = $cgi->param('fileToUpload');
    my $upload_dir = '/uploads';
    my $filepath = "$upload_dir/" . basename($filename);

    open(my $out, '>', $filepath) or die "Impossible open file: $!";
    while (my $bytesread = <$filehandle>) 
    {
        print $out $bytesread;
    }
    close($out);

    print "<html><body>";
    print "<h1>Your file '$filename' has been uploaded successfully. accept!</h1>";
    print "<p></p>";
    print "</body></html>";
} 
else 
{
    print "<html><body>";
    print "<h1>Error Upload</h1>";
    print "</body></html>";
}