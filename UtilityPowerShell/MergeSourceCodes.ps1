# 対象フォルダ（必要ならここを変更）
$targetPath = "C:\Users\K024G\Desktop\A_CG2\NobleEngine\NobleEngine\src\engine"

# 出力ファイル
$outputPath = "C:\Users\K024G\Desktop\A_CG2\NobleEngine\UtilityPowerShell\generated\allfiles.txt"

# UTF-8 (BOMなし) でストリームを開く
$utf8NoBOM = New-Object System.Text.UTF8Encoding($false)
$writer = New-Object System.IO.StreamWriter($outputPath, $false, $utf8NoBOM)

# ファイル列挙（高速化のため -File を使用）
Get-ChildItem $targetPath -Recurse -File | ForEach-Object {

    $writer.WriteLine("========================================")
    $writer.WriteLine("=== FILE: $($_.FullName) ===")
    $writer.WriteLine("========================================")

    # 巨大ファイルでも安全に読み込むために StreamReader を使用
    $reader = [System.IO.StreamReader]::new($_.FullName)
    while (($line = $reader.ReadLine()) -ne $null) {
        $writer.WriteLine($line)
    }
    $reader.Close()

    # 区切り
    $writer.WriteLine("`n`n")
}

$writer.Close()
