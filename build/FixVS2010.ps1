function CreateTargetName($xml, $configuration)
{
  $uri = "http://schemas.microsoft.com/developer/msbuild/2003"

  [System.Xml.XmlNamespaceManager] $nsmgr = $xml.NameTable;
  $nsmgr.AddNamespace("msb", $uri);

  $propertyGroup = $xml.SelectSingleNode("//msb:PropertyGroup[contains(@Condition, '$configuration') and msb:OutDir]", $nsmgr)
  if ($propertyGroup -eq $null)
  {
    return
  }

  $targetName = $propertyGroup.SelectSingleNode("msb:TargetName", $nsmgr)
  if ($targetName -ne $null)
  {
    return;
  }

  $outputFile = $xml.SelectSingleNode("//msb:ItemDefinitionGroup[contains(@Condition, '$configuration')]//msb:OutputFile", $nsmgr)
  if ($outputFile -eq $null)
  {
    return
  }

  $i = $outputFile.InnerText.LastIndexOf("\") + 1
  $j = $outputFile.InnerText.LastIndexOf(".")
  $name = $outputFile.InnerText.SubString($i, $j-$i)

  $targetName = $xml.CreateElement("TargetName", $uri)
  $targetName.InnerText = $name
  [void]$propertyGroup.AppendChild($targetName)

  [void]$outputFile.ParentNode.RemoveChild($outputFile)
}

function FixProjectFile($fileName)
{
  Write-Host "Fixing: $fileName"

  $xml = [xml](get-content $fileName)
  CreateTargetName $xml "Debug"
  CreateTargetName $xml "Release"
  $xml.Save($fileName)
}

function FixProjectFiles()
{
  foreach ($fileName in [IO.Directory]::GetFiles(".", "*.vcxproj", [IO.SearchOption]::AllDirectories))
  {
    FixProjectFile($fileName)
  }
}

FixProjectFiles
