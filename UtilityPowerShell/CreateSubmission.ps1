# 提出用フォルダ一式（実行ファイル / ソースコード・アセット一覧 / ReadMe.md）をデスクトップに作成するスクリプト
# 使い方: このファイルを右クリック→「PowerShellで実行」
#         ターミナルからなら: powershell -ExecutionPolicy Bypass -File .\UtilityPowerShell\CreateSubmission.ps1
# ※ 日本語を含むので「UTF-8 (BOM付き)」で保存すること

# エラーが起きたらその場で止める（Copy-Item の失敗を見逃さないため）
$ErrorActionPreference = "Stop"

try {
    # ===== 設定（必要ならここを変更） =====
    # このスクリプトは <プロジェクト>\UtilityPowerShell\ にある前提。1つ上の階層をプロジェクトルートにする
    $projectRoot     = Split-Path $PSScriptRoot -Parent
    $releaseDir      = Join-Path $projectRoot "generated\outputs\Release"
    $submissionName  = "LE3A_18_ヨコヤマ_タダノブ"

    $desktop         = [Environment]::GetFolderPath("Desktop")
    $submissionRoot  = Join-Path $desktop $submissionName
    $exeFolder       = Join-Path $submissionRoot "実行ファイル"
    $srcFolder       = Join-Path $submissionRoot "実行可能なソースコード・アセット一覧"
    $readmePath      = Join-Path $submissionRoot "ReadMe.md"

    # ===== 事前チェック（既存フォルダを消す前に、必要なファイルが揃っているか確認） =====
    $requiredPaths = @(
        (Join-Path $releaseDir  "NobleEngine.exe"),
        (Join-Path $releaseDir  "dxcompiler.dll"),
        (Join-Path $releaseDir  "dxil.dll"),
        (Join-Path $projectRoot "NobleEngine\assets"),
        (Join-Path $projectRoot "NobleEngine.slnx"),
        (Join-Path $projectRoot "README.md"),
        (Join-Path $projectRoot ".editorconfig")
    )
    $missing = @($requiredPaths | Where-Object { -not (Test-Path -LiteralPath $_) })
    if ($missing.Count -gt 0) {
        throw ("次のファイルが見つかりません（Releaseビルド済みか確認してください）:`n  " + ($missing -join "`n  "))
    }

    # ===== 既存の提出フォルダがあれば作り直す（デスクトップ上の生成物のみが対象） =====
    if (Test-Path -LiteralPath $submissionRoot) {
        Write-Host "既存の '$submissionName' を削除して作り直します..." -ForegroundColor Yellow
        Remove-Item -LiteralPath $submissionRoot -Recurse -Force
    }

    New-Item -ItemType Directory -Path $submissionRoot | Out-Null
    New-Item -ItemType Directory -Path $exeFolder | Out-Null
    New-Item -ItemType Directory -Path $srcFolder | Out-Null

    # ===== 要求1: ReadMe.md（空ファイルとして作成。内容は各自で記入） =====
    New-Item -ItemType File -Path $readmePath | Out-Null

    # ===== 要求2: 実行ファイル一式 =====
    Copy-Item (Join-Path $releaseDir "NobleEngine.exe")    -Destination $exeFolder
    Copy-Item (Join-Path $releaseDir "dxcompiler.dll")     -Destination $exeFolder
    Copy-Item (Join-Path $releaseDir "dxil.dll")           -Destination $exeFolder
    Copy-Item (Join-Path $projectRoot "NobleEngine\assets") -Destination $exeFolder -Recurse

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
    # robocopy は 0～7 が成功、8 以上が失敗（外部コマンドなので $ErrorActionPreference では止まらない）
    if ($LASTEXITCODE -ge 8) {
        throw "robocopy が失敗しました（終了コード: $LASTEXITCODE）"
    }

    Copy-Item (Join-Path $projectRoot "NobleEngine.slnx") -Destination $srcFolder
    Copy-Item (Join-Path $projectRoot "README.md")        -Destination $srcFolder
    Copy-Item (Join-Path $projectRoot ".editorconfig")    -Destination $srcFolder

    Write-Host "提出用フォルダを作成しました: $submissionRoot" -ForegroundColor Green
}
catch {
    Write-Host "エラー: $($_.Exception.Message)" -ForegroundColor Red
}
finally {
    # 右クリック実行だとウィンドウがすぐ閉じてエラーが読めないので、Enterを押すまで待つ
    Read-Host "Enterキーで終了します"
}