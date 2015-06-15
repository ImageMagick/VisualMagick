###############################################################################
# Copyright ImageMagick 2015
###############################################################################

Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem

function Add-File-To-Zip($archive, $path, $name)
{
  $entry = $archive.CreateEntry($name)
  $fs = $entry.Open()
  $output = [System.IO.StreamWriter] $fs
  $input = [System.IO.File]::OpenRead($path)
  $buffer = New-Object Byte[] 32768
  $length = $input.Read($buffer, 0, $buffer.Length)
  while($length -gt 0)
  {
    $output.Write($buffer, 0, $length)
    $length = $input.Read($buffer, 0, $buffer.Length)
  }
  $input.Dispose();
  $output.Dispose();
}

function Add-Path-To-Zip($archive, $path, $name)
{
  $dir = Get-Item $path
  if ($dir -is [System.IO.DirectoryInfo])
  {
    ForEach ($file in $dir.GetFiles())
    {
      Add-File-To-Zip $archive $file.FullName "$name\$($file.Name)"
    }
    ForEach ($subDir in $dir.GetDirectories())
    {
      Add-Path-To-Zip $archive $subDir.FullName "$name\$($subDir.Name)"
    }
  }
  else
  {
    Add-File-To-Zip $archive $path $name
  }
}

function Create-Zip-File($fileName, $files)
{
  $archive = [System.IO.Compression.ZipFile]::Open($fileName, [System.IO.Compression.ZipArchiveMode]::Create)
  ForEach ($file in $files)
  {
    Add-Path-To-Zip $archive $file $file
  }
  $archive.Dispose()
}

Create-Zip-File $args[0] $args[1..($args.length-1)]
