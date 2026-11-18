# エンジンの既定テクスチャ（uvChecker.png / circle2.png）を自前で生成するスクリプト
#
# 使い方:
#   pwsh -File .\UtilityPowerShell\CreateTextures.ps1
#   pwsh -File .\UtilityPowerShell\CreateTextures.ps1 -OutputDir C:\temp\tex   ← 試しに別の場所へ出す
#
# ※ 日本語を含むので「UTF-8 (BOM付き)」で保存すること（Windows PowerShell 5.1 でも動かすため）

param(
    # 省略すると <スクリプトの1つ上>\NobleEngine\assets\engine\texture に書き出す
    [string]$OutputDir = ""
)

$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

if ($OutputDir -eq "") {
    $OutputDir = Join-Path (Split-Path $PSScriptRoot -Parent) "NobleEngine\assets\engine\texture"
}
$particleDir = Join-Path $OutputDir "particle"
New-Item -ItemType Directory -Force -Path $OutputDir, $particleDir | Out-Null

# HSV(色相 0～360) → Color
function Get-HsvColor([double]$h, [double]$s, [double]$v) {
    $c = $v * $s
    $x = $c * (1 - [Math]::Abs((($h / 60) % 2) - 1))
    $m = $v - $c
    switch ([int][Math]::Floor($h / 60) % 6) {
        0 { $r = $c; $g = $x; $b = 0 }
        1 { $r = $x; $g = $c; $b = 0 }
        2 { $r = 0;  $g = $c; $b = $x }
        3 { $r = 0;  $g = $x; $b = $c }
        4 { $r = $x; $g = 0;  $b = $c }
        default { $r = $c; $g = 0; $b = $x }
    }
    return [System.Drawing.Color]::FromArgb(255, [int](($r + $m) * 255), [int](($g + $m) * 255), [int](($b + $m) * 255))
}

# ===== uvChecker.png : 512x512、8x8マス、マスごとに色相をずらし「x,y」を書く =====
function New-UvChecker([string]$path) {
    $size  = 512
    $cells = 8
    $cell  = $size / $cells

    $bmp = New-Object System.Drawing.Bitmap $size, $size, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode     = [System.Drawing.Drawing2D.SmoothingMode]::None
    $g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit

    $font   = New-Object System.Drawing.Font "Segoe UI", 15, ([System.Drawing.FontStyle]::Bold), ([System.Drawing.GraphicsUnit]::Pixel)
    $format = New-Object System.Drawing.StringFormat
    $format.Alignment     = [System.Drawing.StringAlignment]::Center
    $format.LineAlignment = [System.Drawing.StringAlignment]::Center
    $textBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 20, 20, 20))
    $fineLine  = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(70, 255, 255, 255)), 1
    $edgeLine  = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(140, 255, 255, 255)), 1

    for ($y = 0; $y -lt $cells; $y++) {
        for ($x = 0; $x -lt $cells; $x++) {
            # 斜めに同じ色が並ぶよう (x - y) で色相を決める（向きや反転が一目で分かる）
            $hue   = ((($x - $y) % $cells + $cells) % $cells) * (360.0 / $cells)
            $brush = New-Object System.Drawing.SolidBrush (Get-HsvColor $hue 0.55 1.0)
            $g.FillRectangle($brush, $x * $cell, $y * $cell, $cell, $cell)
            $brush.Dispose()

            # マスの中の細かい格子（1マスを8分割）
            for ($i = 1; $i -lt 8; $i++) {
                $p = $i * $cell / 8
                $g.DrawLine($fineLine, $x * $cell + $p, $y * $cell, $x * $cell + $p, ($y + 1) * $cell - 1)
                $g.DrawLine($fineLine, $x * $cell, $y * $cell + $p, ($x + 1) * $cell - 1, $y * $cell + $p)
            }
            $g.DrawRectangle($edgeLine, $x * $cell, $y * $cell, $cell - 1, $cell - 1)

            $rect = New-Object System.Drawing.RectangleF ([single]($x * $cell)), ([single]($y * $cell)), ([single]$cell), ([single]$cell)
            $g.DrawString("$x,$y", $font, $textBrush, $rect, $format)
        }
    }

    $g.Dispose(); $font.Dispose(); $format.Dispose(); $textBrush.Dispose(); $fineLine.Dispose(); $edgeLine.Dispose()
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()
}

# ===== circle2.png : 512x512、中心は白い円、外側へ向かってやわらかく消える =====
function New-SoftCircle([string]$path) {
    $size   = 512
    $center = ($size - 1) / 2.0
    $core   = $size * 0.25        # ここまでは完全に不透明（半径128px）
    $fade   = $size * 0.53        # ここで透明になる（角は必ず透明）

    $bmp  = New-Object System.Drawing.Bitmap $size, $size, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $rect = New-Object System.Drawing.Rectangle 0, 0, $size, $size
    $data = $bmp.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::WriteOnly, $bmp.PixelFormat)
    $bytes = New-Object byte[] ($data.Stride * $size)

    for ($y = 0; $y -lt $size; $y++) {
        for ($x = 0; $x -lt $size; $x++) {
            $r = [Math]::Sqrt(($x - $center) * ($x - $center) + ($y - $center) * ($y - $center))
            if ($r -le $core)     { $a = 1.0 }
            elseif ($r -ge $fade) { $a = 0.0 }
            else {
                $t = ($r - $core) / ($fade - $core)
                $a = (1.0 - $t) * (1.0 - $t)   # 外ほど急に薄くなる
            }
            $v = [byte][Math]::Round($a * 255)
            $i = $y * $data.Stride + $x * 4
            # 元の circle2 と同じく色も明るさに合わせて暗くする（B, G, R, A の順）
            $bytes[$i] = $v; $bytes[$i + 1] = $v; $bytes[$i + 2] = $v; $bytes[$i + 3] = $v
        }
    }

    [System.Runtime.InteropServices.Marshal]::Copy($bytes, 0, $data.Scan0, $bytes.Length)
    $bmp.UnlockBits($data)
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()
}

$uvPath     = Join-Path $OutputDir "uvChecker.png"
$circlePath = Join-Path $particleDir "circle2.png"
New-UvChecker  $uvPath
New-SoftCircle $circlePath
Write-Host "作成しました:"
Write-Host "  $uvPath"
Write-Host "  $circlePath"
