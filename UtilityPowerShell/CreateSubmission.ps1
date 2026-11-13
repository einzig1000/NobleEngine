# 提出用フォルダ一式（実行ファイル / 実行可能なソースコード・アセット一覧 / ReadMe.md）を作るスクリプト
#
# 使い方:
#   ・このファイルを右クリック →「PowerShell で実行」
#   ・ターミナルから: powershell -ExecutionPolicy Bypass -File .\UtilityPowerShell\CreateSubmission.ps1
#
# オプション（ターミナルから実行するときだけ）:
#   -Build      : exe があっても毎回 Release ビルドしてから作る
#   -OutputDir  : 出力先フォルダ（省略するとデスクトップ）
#   -NoPrompt   : 途中の質問と最後の Enter 待ちをしない
#
# ※ 日本語を含むので「UTF-8 (BOM付き)」で保存すること（BOM が無いと Windows PowerShell 5.1 で文字化けして動かない）

param(
    [switch]$Build,
    [string]$OutputDir = "",
    [switch]$NoPrompt
)

# エラーが起きたらその場で止める（コピーの失敗を見逃さないため）
$ErrorActionPreference = "Stop"

# ===== 設定（必要ならここを変更） =====
$submissionName    = "LE3A_18_ヨコヤマ_タダノブ"      # 提出フォルダの名前
$projectFolderName = "NobleEngine"                    # .vcxproj と assets が入っているフォルダ
$configuration     = "Release"
$platform          = "x64"
$submissionExeName = ""                               # 提出物の exe 名を変えたいときだけ指定（例: "MyCraftBuilders.exe"）。空ならビルドされた名前のまま
$rootFiles         = @("README.md", ".editorconfig")  # ソリューションと一緒にソース側へコピーするファイル

# ===== 小さな道具 =====

# Enter / y なら $true。-NoPrompt のときや入力できない環境では $false
function Read-YesNo([string]$message) {
    if ($NoPrompt) { return $false }
    try { $answer = Read-Host "$message [Y/n]" } catch { return $false }
    if ($null -eq $answer) { return $false }
    return ($answer -match '^\s*$' -or $answer -match '^\s*(y|yes|ｙ|はい)')
}

# スクリプトの場所から上にたどって、ソリューション(.slnx/.sln)と $projectFolderName があるフォルダを探す
function Test-ProjectRoot([string]$dir) {
    $solutions = @(Get-ChildItem -LiteralPath $dir -File -ErrorAction SilentlyContinue | Where-Object { $_.Extension -in ".slnx", ".sln" })
    return ($solutions.Count -gt 0 -and (Test-Path -LiteralPath (Join-Path $dir $projectFolderName) -PathType Container))
}

# git を実行して出力を行ごとに返す。git が無い・リポジトリでない等で失敗したら $null
function Invoke-GitLines([string[]]$arguments) {
    if (-not (Get-Command git -ErrorAction SilentlyContinue)) { return $null }

    # 5.1 では stderr を捨てるだけで例外になるので、この関数の中だけ緩める
    $ErrorActionPreference = "Continue"

    # 日本語のファイル名が文字化けしないよう、git の出力を UTF-8 で受け取る
    $oldEncoding = $null
    try {
        $oldEncoding = [Console]::OutputEncoding
        [Console]::OutputEncoding = New-Object System.Text.UTF8Encoding $false
    } catch { }

    try {
        $lines = & git @arguments 2>$null
        if ($LASTEXITCODE -ne 0) { return $null }
        return ,@($lines)   # 0 件でも「失敗($null)」と区別できるよう配列のまま返す
    }
    finally {
        if ($null -ne $oldEncoding) { try { [Console]::OutputEncoding = $oldEncoding } catch { } }
    }
}

# robocopy に渡す除外リスト
#   git が使える : .gitignore で無視されているもの（= クローンには無いもの）を除外 → 提出物がクローンと同じ中身になる
#   git が使えない: 決め打ちのリストで除外（GitHub の ZIP ダウンロード等。この場合は元々無視対象のファイルがほぼ無い）
function Get-CopyExcludes {
    $ignored = Invoke-GitLines @("-C", $projectRoot, "-c", "core.quotepath=false", "ls-files", "--others", "--ignored", "--exclude-standard", "--directory", "--", $projectFolderName)
    if ($null -eq $ignored) {
        Write-Host "  git が使えないので、決め打ちの除外リスト（Dumps / generated / .vs / imgui.ini / *.user）でコピーします" -ForegroundColor Yellow
        return @{ Dirs = @("Dumps", "generated", ".vs"); Files = @("imgui.ini", "*.user") }
    }

    $dirs  = @()
    $files = @()
    foreach ($line in $ignored) {
        if ([string]::IsNullOrWhiteSpace($line)) { continue }
        # 末尾の \ は robocopy の引数を壊すので取る
        $fullPath = (Join-Path $projectRoot ($line -replace '/', '\')).TrimEnd('\')
        if ($line.EndsWith('/')) { $dirs += $fullPath } else { $files += $fullPath }
    }
    return @{ Dirs = $dirs; Files = $files }
}

# フォルダを丸ごとコピー（robocopy は長いパスや日本語名にも強い）
function Copy-Folder([string]$source, [string]$destination, $excludes) {
    # /R:2 /W:1 … 使用中のファイルで固まらないように（既定だと 30 秒 × 100 万回 待ち続ける）
    $roboArgs = @($source.TrimEnd('\'), $destination.TrimEnd('\'), "/E", "/R:2", "/W:1", "/MT:8", "/NFL", "/NDL", "/NJH", "/NJS", "/NP")
    if ($excludes.Dirs.Count  -gt 0) { $roboArgs += "/XD"; $roboArgs += $excludes.Dirs }
    if ($excludes.Files.Count -gt 0) { $roboArgs += "/XF"; $roboArgs += $excludes.Files }

    $log = & robocopy @roboArgs
    # robocopy は 0～7 が成功、8 以上が失敗（外部コマンドなので $ErrorActionPreference では止まらない）
    if ($LASTEXITCODE -ge 8) {
        $log | Select-Object -Last 15 | ForEach-Object { Write-Host "  $_" }
        throw "コピーに失敗しました（robocopy 終了コード: $LASTEXITCODE）: $source"
    }
}

# C++ が入った Visual Studio の MSBuild.exe を探す。無ければ $null
function Find-MSBuild {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path -LiteralPath $vswhere)) { return $null }
    $found = & $vswhere -latest -prerelease -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find "MSBuild\**\Bin\MSBuild.exe"
    return ($found | Select-Object -First 1)
}

function Invoke-ProjectBuild {
    $msbuild = Find-MSBuild
    if (-not $msbuild) {
        throw "C++ 入りの Visual Studio が見つからないので自動ビルドできません。Visual Studio で $configuration|$platform をビルドしてから再実行してください。"
    }
    Write-Host "  $($solution.Name) を $configuration|$platform でビルドしています（初回は数分かかります）..." -ForegroundColor Cyan
    & $msbuild $solution.FullName "-p:Configuration=$configuration" "-p:Platform=$platform" "-m" "-nologo" "-v:minimal"
    if ($LASTEXITCODE -ne 0) {
        throw "ビルドに失敗しました（MSBuild 終了コード: $LASTEXITCODE）。上に出ているエラーを確認してください。"
    }
}

# 出力フォルダ直下の exe を名前に関係なく探す。複数あればいちばん新しいもの
function Find-BuiltExe {
    if (-not (Test-Path -LiteralPath $outDir)) { return $null }
    $exes = @(Get-ChildItem -LiteralPath $outDir -Filter "*.exe" -File | Sort-Object LastWriteTime -Descending)
    if ($exes.Count -eq 0) { return $null }
    if ($exes.Count -gt 1) {
        Write-Host "  exe が複数あるので、いちばん新しいものを使います:" -ForegroundColor Yellow
        foreach ($e in $exes) { Write-Host ("    {0}  ({1:yyyy/MM/dd HH:mm})" -f $e.Name, $e.LastWriteTime) }
    }
    return $exes[0]
}

# exe が古くないか比べるため、いちばん新しいソースファイルを返す
function Get-NewestSource {
    $items = @(Get-ChildItem -LiteralPath $projectDir -File | Where-Object { $_.Extension -in ".cpp", ".h", ".vcxproj" })
    $srcDir = Join-Path $projectDir "src"
    if (Test-Path -LiteralPath $srcDir) { $items += @(Get-ChildItem -LiteralPath $srcDir -Recurse -File) }
    return ($items | Sort-Object LastWriteTime -Descending | Select-Object -First 1)
}

function Get-FolderSizeMB([string]$path) {
    $sum = (Get-ChildItem -LiteralPath $path -Recurse -File | Measure-Object Length -Sum).Sum
    if ($null -eq $sum) { return 0 }
    return $sum / 1MB
}

# ===== 本体 =====
$failed = $false
try {
    Write-Host "=== 提出用フォルダを作成します ===" -ForegroundColor Cyan

    # ----- 場所を決める -----
    $projectRoot = $PSScriptRoot
    while ($projectRoot -and -not (Test-ProjectRoot $projectRoot)) {
        $projectRoot = Split-Path $projectRoot -Parent
    }
    if (-not $projectRoot) {
        throw "プロジェクトが見つかりません。このスクリプトはプロジェクトの中（ソリューションがあるフォルダかその下）に置いてください。"
    }
    $projectDir = Join-Path $projectRoot $projectFolderName
    $assetsDir  = Join-Path $projectDir "assets"
    $outDir     = Join-Path $projectRoot "generated\outputs\$configuration"
    $solution   = Get-ChildItem -LiteralPath $projectRoot -File |
                  Where-Object { $_.Extension -in ".slnx", ".sln" } |
                  Sort-Object { $_.Extension -ne ".slnx" } |
                  Select-Object -First 1

    if (-not $OutputDir) { $OutputDir = [Environment]::GetFolderPath("Desktop") }
    $OutputDir      = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutputDir)
    $submissionRoot = Join-Path $OutputDir $submissionName
    $exeFolder      = Join-Path $submissionRoot "実行ファイル"
    $srcFolder      = Join-Path $submissionRoot "実行可能なソースコード・アセット一覧"
    $readmePath     = Join-Path $submissionRoot "ReadMe.md"

    Write-Host "  プロジェクト: $projectRoot"
    Write-Host "  出力先      : $submissionRoot"

    if (-not (Test-Path -LiteralPath $assetsDir -PathType Container)) {
        throw "assets フォルダが見つかりません: $assetsDir"
    }
    # 出力先がコピー元の中だと、コピーしたものをまたコピーしてしまう
    if (($submissionRoot + '\').StartsWith($projectDir + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw "出力先が $projectFolderName フォルダの中になっています。別の場所を指定してください。"
    }

    # ----- 1. exe を用意する（無い・古いときはビルドするか聞く） -----
    Write-Host "[1/4] $configuration ビルドの exe を確認しています..."
    if ($Build) { Invoke-ProjectBuild }
    $exe = Find-BuiltExe
    if (-not $Build) {
        if (-not $exe) {
            Write-Host "  $outDir に exe がありません。" -ForegroundColor Yellow
            if (Read-YesNo "  今 $configuration ビルドしますか？") {
                Invoke-ProjectBuild
                $exe = Find-BuiltExe
            }
        }
        else {
            $newest = Get-NewestSource
            if ($newest -and $newest.LastWriteTime -gt $exe.LastWriteTime) {
                Write-Host ("  exe ({0:MM/dd HH:mm}) がソースの最終更新 ({1:MM/dd HH:mm} {2}) より古いです。" -f $exe.LastWriteTime, $newest.LastWriteTime, $newest.Name) -ForegroundColor Yellow
                if (Read-YesNo "  ビルドし直しますか？（n なら今の exe のまま続けます）") {
                    Invoke-ProjectBuild
                    $exe = Find-BuiltExe
                }
            }
        }
    }
    if (-not $exe) {
        throw "exe が見つかりません: $outDir`n  Visual Studio で $configuration|$platform をビルドしてから再実行してください（ターミナルからなら -Build を付けると自動でビルドします）。"
    }
    $exeDestName = $exe.Name
    if ($submissionExeName) {
        $exeDestName = $submissionExeName
        if (-not $exeDestName.EndsWith(".exe", [StringComparison]::OrdinalIgnoreCase)) { $exeDestName += ".exe" }
    }
    Write-Host ("  使う exe: {0} ({1:yyyy/MM/dd HH:mm})" -f $exe.Name, $exe.LastWriteTime)

    # ----- 2. 提出フォルダを用意する（作り直すのは2つの生成フォルダだけ。ReadMe.md など他のファイルは残す） -----
    Write-Host "[2/4] 提出用フォルダを準備しています..."
    foreach ($folder in @($exeFolder, $srcFolder)) {
        if (Test-Path -LiteralPath $folder) {
            try {
                Remove-Item -LiteralPath $folder -Recurse -Force
            }
            catch {
                throw "前回の '$(Split-Path $folder -Leaf)' を消せませんでした。そこからゲームを起動していたり、エクスプローラー等で中を開いていたら閉じてから再実行してください。`n  ($($_.Exception.Message))"
            }
        }
        [IO.Directory]::CreateDirectory($folder) | Out-Null
    }
    if (Test-Path -LiteralPath $readmePath) {
        Write-Host "  ReadMe.md は既にあるので、そのまま残します"
    }
    else {
        [IO.File]::WriteAllText($readmePath, "")
        Write-Host "  ReadMe.md を空で作りました（内容は各自で記入）"
    }

    # ----- 3. 実行ファイル一式（exe + 同じフォルダの dll + assets） -----
    Write-Host "[3/4] 実行ファイル一式をコピーしています..."
    Copy-Item -LiteralPath $exe.FullName -Destination (Join-Path $exeFolder $exeDestName)
    $dlls = @(Get-ChildItem -LiteralPath $outDir -Filter "*.dll" -File)
    foreach ($dll in $dlls) { Copy-Item -LiteralPath $dll.FullName -Destination $exeFolder }
    if ($dlls.Count -eq 0) {
        Write-Host "  $outDir に dll が 1 つもありません（dxcompiler.dll / dxil.dll が無いと起動できないかもしれません）" -ForegroundColor Yellow
    }
    $excludes = Get-CopyExcludes
    Copy-Folder $assetsDir (Join-Path $exeFolder "assets") $excludes

    # ----- 4. 実行可能なソースコード・アセット一覧 -----
    Write-Host "[4/4] ソースコード・アセットをコピーしています..."
    Copy-Folder $projectDir (Join-Path $srcFolder $projectFolderName) $excludes
    Copy-Item -LiteralPath $solution.FullName -Destination $srcFolder
    foreach ($name in $rootFiles) {
        $path = Join-Path $projectRoot $name
        if (Test-Path -LiteralPath $path) { Copy-Item -LiteralPath $path -Destination $srcFolder }
        else { Write-Host "  $name が無いのでスキップしました" -ForegroundColor Yellow }
    }

    Write-Host ""
    Write-Host "提出用フォルダを作成しました: $submissionRoot" -ForegroundColor Green
    Write-Host ("  実行ファイル: {0} ほか ({1:N0} MB)" -f $exeDestName, (Get-FolderSizeMB $exeFolder))
    Write-Host ("  ソース一式  : {0:N0} MB" -f (Get-FolderSizeMB $srcFolder))
    if (-not $NoPrompt) { Invoke-Item -LiteralPath $submissionRoot }
}
catch {
    $failed = $true
    Write-Host ""
    Write-Host "エラー: $($_.Exception.Message)" -ForegroundColor Red
}
finally {
    # 右クリック実行だとウィンドウがすぐ閉じてエラーが読めないので、Enter を押すまで待つ
    if (-not $NoPrompt) {
        try { Read-Host "Enterキーで終了します" | Out-Null } catch { }
    }
}
# 終了コード（成功 0 / 失敗 1）。書かないと robocopy の終了コードが残って紛らわしい
if ($failed) { exit 1 }
exit 0