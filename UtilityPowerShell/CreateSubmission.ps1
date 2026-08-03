# 提出用フォルダ一式（実行ファイル / ソースコード・アセット一覧 / ReadMe.md）をデスクトップに作成するスクリプト
# 使い方: PowerShellでこのファイルを右クリック→「PowerShellで実行」、もしくは `.\CreateSubmission.ps1`

# ===== 設定（必要ならここを変更） =====
$projectRoot     = "C:\Users\K024G\Desktop\A_CG2\NobleEngine"
$releaseDir      = Join-Path $projectRoot "generated\outputs\Release"
$submissionName  = "LE3A_18_ヨコヤマ_タダノブ"

$desktop         = [Environment]::GetFolderPath("Desktop")
$submissionRoot  = Join-Path $desktop $submissionName
$exeFolder       = Join-Path $submissionRoot "実行ファイル"
$srcFolder       = Join-Path $submissionRoot "実行可能なソースコード・アセット一覧"
$readmePath      = Join-Path $submissionRoot "ReadMe.md"

# ===== 既存の提出フォルダがあれば作り直す（デスクトップ上の生成物のみが対象） =====
if (Test-Path $submissionRoot) {
    Write-Host "既存の '$submissionName' を削除して作り直します..." -ForegroundColor Yellow
    Remove-Item $submissionRoot -Recurse -Force
}

New-Item -ItemType Directory -Path $submissionRoot | Out-Null
New-Item -ItemType Directory -Path $exeFolder | Out-Null
New-Item -ItemType Directory -Path $srcFolder | Out-Null

# ===== 要求1: ReadMe.md（空ファイルとして作成。内容は各自で記入） =====
New-Item -ItemType File -Path $readmePath | Out-Null

# ===== 要求2: 実行ファイル一式 =====
Copy-Item (Join-Path $releaseDir "NobleEngine.exe")  -Destination $exeFolder
Copy-Item (Join-Path $releaseDir "dxcompiler.dll")    -Destination $exeFolder
Copy-Item (Join-Path $releaseDir "dxil.dll")          -Destination $exeFolder
Copy-Item (Join-Path $projectRoot "NobleEngine\resources") -Destination $exeFolder -Recurse

# ===== 要求3・4: 実行可能なソースコード・アセット一覧 =====
# NobleEngineフォルダは Dumps / generated / imgui.ini を除外してコピー
$roboArgs = @(
    (Join-Path $projectRoot "NobleEngine"),
    (Join-Path $srcFolder "NobleEngine"),
    "/E",
    "/XD", "Dumps", "generated",
    "/XF", "imgui.ini",
    "/NFL", "/NDL", "/NJH", "/NJS"
)
robocopy @roboArgs | Out-Null

Copy-Item (Join-Path $projectRoot "NobleEngine.slnx") -Destination $srcFolder
Copy-Item (Join-Path $projectRoot "README.md")         -Destination $srcFolder
Copy-Item (Join-Path $projectRoot ".editorconfig")     -Destination $srcFolder

Write-Host "提出用フォルダを作成しました: $submissionRoot" -ForegroundColor Green
