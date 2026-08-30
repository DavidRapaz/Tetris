<#
.SYNOPSIS
    Gera o conjunto de assets graficos do Tetris (blocos, paineis, fundo, logo, teclas).

.DESCRIPTION
    Todos os assets sao desenhados por codigo com System.Drawing, para que a paleta e
    o estilo possam ser afinados num sitio so e re-gerados a qualquer momento.

    As medidas seguem a grelha do jogo: PIECE_SIZE = 30, NEXT_PIECE_SIZE = 15,
    tabuleiro 10x20 (300x600), painel das proximas pecas 9x18 (135x270),
    janela 1280x760.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File tools/generate_assets.ps1
#>

[CmdletBinding()]
param(
    [string] $OutputRoot
)

$ErrorActionPreference = 'Stop'

if (-not $OutputRoot)
{
    $scriptDir  = Split-Path -Parent $MyInvocation.MyCommand.Path
    $OutputRoot = Join-Path (Split-Path -Parent $scriptDir) 'assets'
}

Add-Type -AssemblyName System.Drawing

# ---------------------------------------------------------------- CONFIGURACAO

$PieceSize     = 30    # PIECE_SIZE do Game.cpp
$NextPieceSize = 15    # NEXT_PIECE_SIZE do Game.cpp
$Supersample   = 8     # fator de anti-aliasing por sobre-amostragem

# Paleta das pecas. A chave e o nome do ficheiro, o valor a cor base.
$Palette = [ordered]@{
    'i_cyan'   = '#22C9DE'
    'o_yellow' = '#EFC030'
    't_purple' = '#A855F7'
    's_green'  = '#3FCF6A'
    'z_red'    = '#EF4444'
    'j_blue'   = '#3B82F6'
    'l_orange' = '#F97316'
}

# Nomes usados pelo codigo actual (Color.h) -> peca equivalente na paleta nova.
$Aliases = [ordered]@{
    'red'       = 'z_red'
    'green'     = 's_green'
    'lightblue' = 'i_cyan'
    'orange'    = 'l_orange'
}

# Cores da interface
$InkDeep   = '#05070B'   # fundo, canto exterior
$InkBase   = '#0B0F16'   # fundo, centro
$PanelFill = '#0A0E15'
$PanelEdge = '#1D2938'
$Accent    = '#22C9DE'

# ------------------------------------------------------------------- UTILITARIOS

function ConvertTo-Color
{
    param([string] $Hex, [int] $Alpha = 255)

    $clean = $Hex.TrimStart('#')
    $r = [Convert]::ToInt32($clean.Substring(0, 2), 16)
    $g = [Convert]::ToInt32($clean.Substring(2, 2), 16)
    $b = [Convert]::ToInt32($clean.Substring(4, 2), 16)

    return [System.Drawing.Color]::FromArgb($Alpha, $r, $g, $b)
}

function Get-Shade
{
    <# Clareia (factor > 1) ou escurece (factor < 1) uma cor. #>
    param([System.Drawing.Color] $Color, [double] $Factor, [int] $Alpha = -1)

    $clamp = {
        param($value)
        [int][Math]::Max(0, [Math]::Min(255, [Math]::Round($value)))
    }

    if ($Factor -ge 1)
    {
        $t = $Factor - 1
        $r = & $clamp ($Color.R + (255 - $Color.R) * $t)
        $g = & $clamp ($Color.G + (255 - $Color.G) * $t)
        $b = & $clamp ($Color.B + (255 - $Color.B) * $t)
    }
    else
    {
        $r = & $clamp ($Color.R * $Factor)
        $g = & $clamp ($Color.G * $Factor)
        $b = & $clamp ($Color.B * $Factor)
    }

    if ($Alpha -lt 0) { $a = $Color.A } else { $a = $Alpha }
    return [System.Drawing.Color]::FromArgb($a, $r, $g, $b)
}

function New-Canvas
{
    param([int] $Width, [int] $Height)

    $bitmap = New-Object System.Drawing.Bitmap($Width, $Height, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g      = [System.Drawing.Graphics]::FromImage($bitmap)

    $g.SmoothingMode     = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g.PixelOffsetMode   = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
    $g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
    $g.Clear([System.Drawing.Color]::Transparent)

    return [pscustomobject]@{ Bitmap = $bitmap; Graphics = $g }
}

function New-RoundedPath
{
    param([double] $X, [double] $Y, [double] $Width, [double] $Height, [double] $Radius)

    $path = New-Object System.Drawing.Drawing2D.GraphicsPath
    $d    = [float](2 * $Radius)

    if ($Radius -le 0)
    {
        $path.AddRectangle((New-Object System.Drawing.RectangleF($X, $Y, $Width, $Height)))
        return $path
    }

    $path.AddArc([float]$X,                  [float]$Y,                   $d, $d, 180, 90)
    $path.AddArc([float]($X + $Width - $d),  [float]$Y,                   $d, $d, 270, 90)
    $path.AddArc([float]($X + $Width - $d),  [float]($Y + $Height - $d),  $d, $d, 0,   90)
    $path.AddArc([float]$X,                  [float]($Y + $Height - $d),  $d, $d, 90,  90)
    $path.CloseFigure()

    return $path
}

function Resize-Bitmap
{
    param([System.Drawing.Bitmap] $Source, [int] $Width, [int] $Height)

    $canvas = New-Canvas -Width $Width -Height $Height
    $canvas.Graphics.DrawImage($Source, (New-Object System.Drawing.Rectangle(0, 0, $Width, $Height)))
    $canvas.Graphics.Dispose()

    return $canvas.Bitmap
}

function Save-Png
{
    param([System.Drawing.Bitmap] $Bitmap, [string] $Path)

    $dir = Split-Path -Parent $Path
    if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir -Force | Out-Null }

    $Bitmap.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
    Write-Host ("  {0,-46} {1}x{2}" -f (Split-Path -Leaf $Path), $Bitmap.Width, $Bitmap.Height)
}

function Get-ProjectFont
{
    <# Carrega o Crimson Text do proprio projecto, com fallback para uma fonte do sistema. #>
    param([string] $File, [single] $Size, [System.Drawing.FontStyle] $Style = [System.Drawing.FontStyle]::Regular)

    $path = Join-Path $OutputRoot ("fonts/" + $File)

    if (Test-Path $path)
    {
        if ($null -eq $script:FontCollection)
        {
            $script:FontCollection = New-Object System.Drawing.Text.PrivateFontCollection
        }

        $already = $script:FontCollection.Families | Where-Object { $_.Name -eq 'Crimson Text' }
        if (-not $already) { $script:FontCollection.AddFontFile($path) }

        $family = $script:FontCollection.Families | Where-Object { $_.Name -eq 'Crimson Text' } | Select-Object -First 1
        if ($family) { return New-Object System.Drawing.Font($family, $Size, $Style) }
    }

    return New-Object System.Drawing.Font('Segoe UI', $Size, $Style)
}

# ------------------------------------------------------------------ BLOCO/PECA

function Draw-Block
{
    <#
        Desenha um bloco numa area de 30x30 unidades, escalada por $Scale.
        Estilo: canto arredondado, gradiente vertical, brilho no topo,
        sombra em baixo/direita e contorno escuro.
    #>
    param(
        [System.Drawing.Graphics] $Graphics,
        [System.Drawing.Color]    $Base,
        [double]                  $Scale,
        [double]                  $OffsetX = 0,
        [double]                  $OffsetY = 0
    )

    $s    = $Scale
    $x    = $OffsetX + (1.0 * $s)
    $y    = $OffsetY + (1.0 * $s)
    $size = 28.0 * $s
    $r    = 4.0 * $s

    $top    = Get-Shade $Base 1.30
    $bottom = Get-Shade $Base 0.78
    $edge   = Get-Shade $Base 0.42

    # Corpo com gradiente vertical
    $body = New-RoundedPath -X $x -Y $y -Width $size -Height $size -Radius $r
    $rect = New-Object System.Drawing.RectangleF([float]$x, [float]($y - 1), [float]$size, [float]($size + 2))
    $fill = New-Object System.Drawing.Drawing2D.LinearGradientBrush($rect, $top, $bottom, 90.0)
    $Graphics.FillPath($fill, $body)
    $fill.Dispose()

    # Sombra interior em baixo/direita
    $shadow = New-RoundedPath -X ($x + 1.5 * $s) -Y ($y + 1.5 * $s) -Width ($size - 3 * $s) -Height ($size - 3 * $s) -Radius ($r * 0.7)
    $penShadow = New-Object System.Drawing.Pen((Get-Shade $Base 0.55 90), [float](2.0 * $s))
    $Graphics.DrawPath($penShadow, $shadow)
    $penShadow.Dispose()
    $shadow.Dispose()

    # Brilho no topo (faceta superior)
    $glossRect = New-Object System.Drawing.RectangleF([float]($x + 2.5 * $s), [float]($y + 2.5 * $s), [float]($size - 5 * $s), [float](10.0 * $s))
    $glossPath = New-RoundedPath -X ($x + 2.5 * $s) -Y ($y + 2.5 * $s) -Width ($size - 5 * $s) -Height (10.0 * $s) -Radius ($r * 0.6)
    $gloss = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
        $glossRect,
        [System.Drawing.Color]::FromArgb(70, 255, 255, 255),
        [System.Drawing.Color]::FromArgb(0, 255, 255, 255),
        90.0)
    $Graphics.FillPath($gloss, $glossPath)
    $gloss.Dispose()
    $glossPath.Dispose()

    # Realce interior
    $inner = New-RoundedPath -X ($x + 1.2 * $s) -Y ($y + 1.2 * $s) -Width ($size - 2.4 * $s) -Height ($size - 2.4 * $s) -Radius ($r * 0.8)
    $penInner = New-Object System.Drawing.Pen((Get-Shade $Base 1.55 110), [float](1.2 * $s))
    $Graphics.DrawPath($penInner, $inner)
    $penInner.Dispose()
    $inner.Dispose()

    # Contorno exterior
    $penEdge = New-Object System.Drawing.Pen($edge, [float](1.4 * $s))
    $Graphics.DrawPath($penEdge, $body)
    $penEdge.Dispose()
    $body.Dispose()
}

function New-BlockBitmap
{
    param([System.Drawing.Color] $Base, [int] $Size)

    $hi = New-Canvas -Width ($Size * $Supersample) -Height ($Size * $Supersample)
    Draw-Block -Graphics $hi.Graphics -Base $Base -Scale ($Size * $Supersample / 30.0)
    $hi.Graphics.Dispose()

    $out = Resize-Bitmap -Source $hi.Bitmap -Width $Size -Height $Size
    $hi.Bitmap.Dispose()

    return $out
}

function New-GhostBitmap
{
    <# Bloco "fantasma" para a pre-visualizacao de queda. #>
    param([System.Drawing.Color] $Base, [int] $Size)

    $s  = $Size * $Supersample / 30.0
    $hi = New-Canvas -Width ($Size * $Supersample) -Height ($Size * $Supersample)
    $g  = $hi.Graphics

    $path = New-RoundedPath -X (1.5 * $s) -Y (1.5 * $s) -Width (27.0 * $s) -Height (27.0 * $s) -Radius (3.5 * $s)

    $brush = New-Object System.Drawing.SolidBrush((Get-Shade $Base 1.0 38))
    $g.FillPath($brush, $path)
    $brush.Dispose()

    $pen = New-Object System.Drawing.Pen((Get-Shade $Base 1.15 210), [float](2.0 * $s))
    $g.DrawPath($pen, $path)
    $pen.Dispose()
    $path.Dispose()

    $g.Dispose()
    $out = Resize-Bitmap -Source $hi.Bitmap -Width $Size -Height $Size
    $hi.Bitmap.Dispose()

    return $out
}

# ------------------------------------------------------------------- GERACAO

Write-Host "Assets em: $OutputRoot" -ForegroundColor Cyan

$imagesRoot = Join-Path $OutputRoot 'images'
$blocksDir  = Join-Path $imagesRoot 'blocks'
$ghostDir   = Join-Path $blocksDir  'ghost'
$smallDir   = Join-Path $blocksDir  'small'
$uiDir      = Join-Path $imagesRoot 'ui'
$keysDir    = Join-Path $imagesRoot 'keys'

# --- Blocos ------------------------------------------------------------------

Write-Host "`nBlocos ($PieceSize px) e variantes:" -ForegroundColor Yellow

$blockBitmaps = @{}

foreach ($name in $Palette.Keys)
{
    $base = ConvertTo-Color $Palette[$name]

    $block = New-BlockBitmap -Base $base -Size $PieceSize
    Save-Png $block (Join-Path $blocksDir "$name.png")
    $blockBitmaps[$name] = $block

    $small = New-BlockBitmap -Base $base -Size $NextPieceSize
    Save-Png $small (Join-Path $smallDir "$name.png")
    $small.Dispose()

    $ghost = New-GhostBitmap -Base $base -Size $PieceSize
    Save-Png $ghost (Join-Path $ghostDir "$name.png")
    $ghost.Dispose()
}

# Nomes compativeis com o codigo actual
Write-Host "`nAliases para os nomes usados no Color.h:" -ForegroundColor Yellow

foreach ($alias in $Aliases.Keys)
{
    $source = $Aliases[$alias]
    Save-Png $blockBitmaps[$source] (Join-Path $blocksDir "$alias.png")
}

# --- Atlas -------------------------------------------------------------------

Write-Host "`nAtlas:" -ForegroundColor Yellow

$atlas = New-Canvas -Width ($PieceSize * $Palette.Count) -Height $PieceSize
$slot  = 0
foreach ($name in $Palette.Keys)
{
    $atlas.Graphics.DrawImage($blockBitmaps[$name], ($slot * $PieceSize), 0, $PieceSize, $PieceSize)
    $slot++
}
$atlas.Graphics.Dispose()
Save-Png $atlas.Bitmap (Join-Path $blocksDir 'atlas.png')
$atlas.Bitmap.Dispose()

# --- Grelha, paineis e fundo -------------------------------------------------

Write-Host "`nInterface:" -ForegroundColor Yellow

# Celula da grelha (30x30) - alinha com a grelha do tabuleiro
$cell = New-Canvas -Width $PieceSize -Height $PieceSize
$cellFill = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(10, 255, 255, 255))
$cell.Graphics.FillRectangle($cellFill, 0, 0, $PieceSize, $PieceSize)
$cellFill.Dispose()
$cellPen = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(26, 255, 255, 255), 1)
$cell.Graphics.DrawLine($cellPen, ($PieceSize - 1), 0, ($PieceSize - 1), ($PieceSize - 1))
$cell.Graphics.DrawLine($cellPen, 0, ($PieceSize - 1), ($PieceSize - 1), ($PieceSize - 1))
$cellPen.Dispose()
$cell.Graphics.Dispose()
Save-Png $cell.Bitmap (Join-Path $uiDir 'grid_cell.png')
$cell.Bitmap.Dispose()

function New-Panel
{
    <# Painel escuro translucido com grelha interior, moldura e cantos de destaque. #>
    param([int] $Width, [int] $Height, [int] $Cell, [switch] $WithGrid)

    $canvas = New-Canvas -Width $Width -Height $Height
    $g      = $canvas.Graphics

    $path = New-RoundedPath -X 0.5 -Y 0.5 -Width ($Width - 1) -Height ($Height - 1) -Radius 6
    $fill = New-Object System.Drawing.SolidBrush((ConvertTo-Color $PanelFill 214))
    $g.FillPath($fill, $path)
    $fill.Dispose()

    if ($WithGrid)
    {
        $gridPen = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(28, 255, 255, 255), 1)
        for ($x = $Cell; $x -lt $Width; $x += $Cell)
        {
            $g.DrawLine($gridPen, $x, 2, $x, ($Height - 3))
        }
        for ($y = $Cell; $y -lt $Height; $y += $Cell)
        {
            $g.DrawLine($gridPen, 2, $y, ($Width - 3), $y)
        }
        $gridPen.Dispose()
    }

    $edgePen = New-Object System.Drawing.Pen((ConvertTo-Color $PanelEdge 255), 1.5)
    $g.DrawPath($edgePen, $path)
    $edgePen.Dispose()
    $path.Dispose()

    # Cantos de destaque
    $accentPen = New-Object System.Drawing.Pen((ConvertTo-Color $Accent 235), 2)
    $len = 16
    $g.DrawLine($accentPen, 1, 8, 1, (8 + $len))                              # topo esquerdo
    $g.DrawLine($accentPen, 8, 1, (8 + $len), 1)
    $g.DrawLine($accentPen, ($Width - 2), 8, ($Width - 2), (8 + $len))        # topo direito
    $g.DrawLine($accentPen, ($Width - 9), 1, ($Width - 9 - $len), 1)
    $g.DrawLine($accentPen, 1, ($Height - 9), 1, ($Height - 9 - $len))        # base esquerda
    $g.DrawLine($accentPen, 8, ($Height - 2), (8 + $len), ($Height - 2))
    $g.DrawLine($accentPen, ($Width - 2), ($Height - 9), ($Width - 2), ($Height - 9 - $len))
    $g.DrawLine($accentPen, ($Width - 9), ($Height - 2), ($Width - 9 - $len), ($Height - 2))
    $accentPen.Dispose()

    $g.Dispose()
    return $canvas.Bitmap
}

$boardPanel = New-Panel -Width ($PieceSize * 10) -Height ($PieceSize * 20) -Cell $PieceSize -WithGrid
Save-Png $boardPanel (Join-Path $uiDir 'panel_board.png')
$boardPanel.Dispose()

$nextPanel = New-Panel -Width ($NextPieceSize * 9) -Height ($NextPieceSize * 18) -Cell $NextPieceSize
Save-Png $nextPanel (Join-Path $uiDir 'panel_next.png')
$nextPanel.Dispose()

# Painel de overlay (Paused / Game Over) - mesmas medidas do RenderRect actual
$overlay = New-Canvas -Width 500 -Height 200
$og      = $overlay.Graphics
$oPath   = New-RoundedPath -X 1 -Y 1 -Width 498 -Height 198 -Radius 12
$oFill   = New-Object System.Drawing.SolidBrush((ConvertTo-Color $InkBase 238))
$og.FillPath($oFill, $oPath)
$oFill.Dispose()
$oPen = New-Object System.Drawing.Pen((ConvertTo-Color $PanelEdge 255), 2)
$og.DrawPath($oPen, $oPath)
$oPen.Dispose()
$oPath.Dispose()
$accentRect = New-Object System.Drawing.RectangleF(120, 2, 260, 3)
$accentBrush = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
    $accentRect,
    (ConvertTo-Color $Accent 0),
    (ConvertTo-Color $Accent 255),
    0.0)
$accentBrush.SetBlendTriangularShape(0.5)
$og.FillRectangle($accentBrush, $accentRect)
$accentBrush.Dispose()
$og.Dispose()
Save-Png $overlay.Bitmap (Join-Path $uiDir 'panel_overlay.png')
$overlay.Bitmap.Dispose()

# Fundo da janela (1280x760)
$bgW = 1280
$bgH = 760
$bg  = New-Canvas -Width $bgW -Height $bgH
$bgG = $bg.Graphics

$bgFill = New-Object System.Drawing.SolidBrush((ConvertTo-Color $InkBase))
$bgG.FillRectangle($bgFill, 0, 0, $bgW, $bgH)
$bgFill.Dispose()

# Brilho radial no centro
$glowPath = New-Object System.Drawing.Drawing2D.GraphicsPath
$glowPath.AddEllipse(-260, -420, ($bgW + 520), ($bgH + 840))
$glow = New-Object System.Drawing.Drawing2D.PathGradientBrush($glowPath)
$glow.CenterPoint       = New-Object System.Drawing.PointF([float]($bgW / 2), [float]($bgH * 0.42))
$glow.CenterColor       = ConvertTo-Color '#141C29' 255
$glow.SurroundColors    = @((ConvertTo-Color $InkDeep 255))
$bgG.FillPath($glow, $glowPath)
$glow.Dispose()
$glowPath.Dispose()

# Grelha ambiente de 30px
$bgGridPen = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(8, 255, 255, 255), 1)
for ($x = 0; $x -lt $bgW; $x += $PieceSize) { $bgG.DrawLine($bgGridPen, $x, 0, $x, $bgH) }
for ($y = 0; $y -lt $bgH; $y += $PieceSize) { $bgG.DrawLine($bgGridPen, 0, $y, $bgW, $y) }
$bgGridPen.Dispose()

$bgG.Dispose()
Save-Png $bg.Bitmap (Join-Path $imagesRoot 'background.png')
$bg.Bitmap.Dispose()

# --- Logo --------------------------------------------------------------------

Write-Host "`nLogo:" -ForegroundColor Yellow

$Glyphs = [ordered]@{
    'T' = @('11111', '00100', '00100', '00100', '00100')
    'E' = @('11111', '10000', '11110', '10000', '11111')
    'R' = @('11110', '10010', '11110', '10100', '10010')
    'I' = @('11111', '00100', '00100', '00100', '11111')
    'S' = @('01111', '10000', '01110', '00001', '11110')
}

$logoWord   = @('T', 'E', 'T', 'R', 'I', 'S')
$logoColors = @('t_purple', 'i_cyan', 's_green', 'z_red', 'o_yellow', 'j_blue')
$logoCell   = 17
$logoW      = 610
$logoH      = 180

$logo  = New-Canvas -Width $logoW -Height $logoH
$logoG = $logo.Graphics

$wordCells = ($logoWord.Count * 5) + ($logoWord.Count - 1)      # 5 colunas por letra + 1 de espaco
$startX    = [int](($logoW - $wordCells * $logoCell) / 2)
$startY    = [int](($logoH - 5 * $logoCell) / 2)

for ($letterIndex = 0; $letterIndex -lt $logoWord.Count; $letterIndex++)
{
    $rows = $Glyphs[$logoWord[$letterIndex]]
    $base = ConvertTo-Color $Palette[$logoColors[$letterIndex]]
    $letterX = $startX + ($letterIndex * 6 * $logoCell)

    for ($row = 0; $row -lt 5; $row++)
    {
        for ($col = 0; $col -lt 5; $col++)
        {
            if ($rows[$row][$col] -ne '1') { continue }

            $cellBmp = New-BlockBitmap -Base $base -Size $logoCell
            $logoG.DrawImage($cellBmp,
                ($letterX + $col * $logoCell),
                ($startY + $row * $logoCell))
            $cellBmp.Dispose()
        }
    }
}

$logoG.Dispose()
Save-Png $logo.Bitmap (Join-Path $imagesRoot 'logo.png')

# Versao 2x para material de divulgacao / README
$logo2x  = New-Canvas -Width ($logoW * 2) -Height ($logoH * 2)
$logo2x.Graphics.DrawImage($logo.Bitmap, 0, 0, ($logoW * 2), ($logoH * 2))
$logo2x.Graphics.Dispose()
Save-Png $logo2x.Bitmap (Join-Path $imagesRoot 'logo@2x.png')
$logo2x.Bitmap.Dispose()
$logo.Bitmap.Dispose()

# --- Teclas ------------------------------------------------------------------

Write-Host "`nTeclas:" -ForegroundColor Yellow

function New-KeyCap
{
    param([int] $Width = 44, [int] $Height = 44, [string] $Label, [string] $Arrow, [single] $FontSize = 17)

    $ss = 4
    $hi = New-Canvas -Width ($Width * $ss) -Height ($Height * $ss)
    $g  = $hi.Graphics

    $inset = 1.5 * $ss
    $path  = New-RoundedPath -X $inset -Y $inset -Width (($Width * $ss) - 2 * $inset) -Height (($Height * $ss) - 2 * $inset) -Radius (7.0 * $ss)

    $rect = New-Object System.Drawing.RectangleF(0, 0, [float]($Width * $ss), [float]($Height * $ss))
    $fill = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
        $rect, (ConvertTo-Color '#232C3B' 255), (ConvertTo-Color '#131A25' 255), 90.0)
    $g.FillPath($fill, $path)
    $fill.Dispose()

    $pen = New-Object System.Drawing.Pen((ConvertTo-Color '#3B4A61' 255), [float](1.5 * $ss))
    $g.DrawPath($pen, $path)
    $pen.Dispose()

    # Brilho no topo
    $glossPath = New-RoundedPath -X (3.5 * $ss) -Y (3.5 * $ss) -Width (($Width - 7) * $ss) -Height (($Height * 0.38) * $ss) -Radius (5.0 * $ss)
    $glossRect = New-Object System.Drawing.RectangleF([float](3.5 * $ss), [float](3.5 * $ss), [float](($Width - 7) * $ss), [float](($Height * 0.38) * $ss))
    $gloss = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
        $glossRect,
        [System.Drawing.Color]::FromArgb(38, 255, 255, 255),
        [System.Drawing.Color]::FromArgb(0, 255, 255, 255),
        90.0)
    $g.FillPath($gloss, $glossPath)
    $gloss.Dispose()
    $glossPath.Dispose()
    $path.Dispose()

    $ink = New-Object System.Drawing.SolidBrush((ConvertTo-Color '#DCE6F2' 255))

    if ($Arrow)
    {
        # Setas desenhadas como poligonos, para nao depender dos glifos da fonte
        $u  = [double] $ss                  # 1 unidade = 1 px na tecla final
        $cx = ($Width * $ss) / 2.0
        $cy = ($Height * $ss) / 2.0

        $head  = 8.0 * $u                   # meia largura da cabeca
        $tip   = 9.0 * $u                   # comprimento da cabeca
        $half  = 2.5 * $u                   # meia espessura do corpo
        $shaft = 9.0 * $u                   # comprimento do corpo

        $point = {
            param($px, $py)
            New-Object System.Drawing.PointF([float]$px, [float]$py)
        }

        switch ($Arrow)
        {
            'down'
            {
                $g.FillRectangle($ink, [float]($cx - $half), [float]($cy - $shaft), [float](2 * $half), [float]$shaft)
                $points = @(
                    (& $point ($cx - $head) $cy),
                    (& $point ($cx + $head) $cy),
                    (& $point $cx           ($cy + $tip)))
            }
            'left'
            {
                $g.FillRectangle($ink, [float]$cx, [float]($cy - $half), [float]$shaft, [float](2 * $half))
                $points = @(
                    (& $point $cx           ($cy - $head)),
                    (& $point $cx           ($cy + $head)),
                    (& $point ($cx - $tip)  $cy))
            }
            'right'
            {
                $g.FillRectangle($ink, [float]($cx - $shaft), [float]($cy - $half), [float]$shaft, [float](2 * $half))
                $points = @(
                    (& $point $cx           ($cy - $head)),
                    (& $point $cx           ($cy + $head)),
                    (& $point ($cx + $tip)  $cy))
            }
        }

        $g.FillPolygon($ink, $points)
    }
    elseif ($Label)
    {
        $font   = Get-ProjectFont -File 'CrimsonText-Bold.ttf' -Size ($FontSize * $ss) -Style ([System.Drawing.FontStyle]::Bold)
        $format = New-Object System.Drawing.StringFormat
        $format.Alignment     = [System.Drawing.StringAlignment]::Center
        $format.LineAlignment = [System.Drawing.StringAlignment]::Center

        $box = New-Object System.Drawing.RectangleF(0, [float](-1.0 * $ss), [float]($Width * $ss), [float]($Height * $ss))
        $g.DrawString($Label, $font, $ink, $box, $format)

        $format.Dispose()
        $font.Dispose()
    }

    $ink.Dispose()
    $g.Dispose()

    $out = Resize-Bitmap -Source $hi.Bitmap -Width $Width -Height $Height
    $hi.Bitmap.Dispose()

    return $out
}

$keys = @(
    @{ File = 'key_left.png';  Arrow = 'left'  },
    @{ File = 'key_right.png'; Arrow = 'right' },
    @{ File = 'key_down.png';  Arrow = 'down'  },
    @{ File = 'key_a.png';     Label = 'A'     },
    @{ File = 'key_d.png';     Label = 'D'     },
    @{ File = 'key_s.png';     Label = 'S'     },
    @{ File = 'key_q.png';     Label = 'Q'     },
    @{ File = 'key_e.png';     Label = 'E'     }
)

foreach ($key in $keys)
{
    if ($key.Arrow) { $bmp = New-KeyCap -Arrow $key.Arrow }
    else            { $bmp = New-KeyCap -Label $key.Label }

    Save-Png $bmp (Join-Path $keysDir $key.File)
    $bmp.Dispose()
}

$esc = New-KeyCap -Width 64 -Height 44 -Label 'ESC' -FontSize 13
Save-Png $esc (Join-Path $keysDir 'key_esc.png')
$esc.Dispose()

$space = New-KeyCap -Width 148 -Height 44 -Label 'SPACE' -FontSize 13
Save-Png $space (Join-Path $keysDir 'key_space.png')
$space.Dispose()

# --- Folha de contacto e mockup ---------------------------------------------

Write-Host "`nPre-visualizacoes:" -ForegroundColor Yellow

# Fora de assets/ de proposito: estas imagens sao para documentacao, nao vao no build
$previewDir = Join-Path (Split-Path -Parent $OutputRoot) 'docs/assets'

# Mockup do ecra de jogo, com o layout real do Game.cpp
$boardX = 500
$boardY = 50

$mock  = New-Canvas -Width $bgW -Height $bgH
$mockG = $mock.Graphics
$mockG.DrawImage([System.Drawing.Image]::FromFile((Join-Path $imagesRoot 'background.png')), 0, 0, $bgW, $bgH)
$mockG.DrawImage([System.Drawing.Image]::FromFile((Join-Path $uiDir 'panel_board.png')), $boardX, $boardY)

$nextX = $boardX + (10 * $PieceSize) + 30
$mockG.DrawImage([System.Drawing.Image]::FromFile((Join-Path $uiDir 'panel_next.png')), $nextX, $boardY)
$mockG.DrawImage([System.Drawing.Image]::FromFile((Join-Path $imagesRoot 'logo.png')), 20, 60, 440, 130)

# Pilha de exemplo no tabuleiro
$stack = @(
    @(17, 0, 'j_blue'), @(17, 1, 'j_blue'), @(17, 2, 'j_blue'), @(17, 3, 'o_yellow'), @(17, 4, 'o_yellow'),
    @(17, 7, 's_green'), @(17, 8, 's_green'), @(17, 9, 't_purple'),
    @(18, 0, 'z_red'), @(18, 1, 'z_red'), @(18, 2, 'o_yellow'), @(18, 3, 'o_yellow'), @(18, 4, 'l_orange'),
    @(18, 5, 'l_orange'), @(18, 6, 'l_orange'), @(18, 7, 's_green'), @(18, 8, 't_purple'), @(18, 9, 't_purple'),
    @(19, 0, 'i_cyan'), @(19, 1, 'i_cyan'), @(19, 2, 'i_cyan'), @(19, 3, 'i_cyan'), @(19, 4, 'j_blue'),
    @(19, 5, 'j_blue'), @(19, 6, 'z_red'), @(19, 7, 'z_red'), @(19, 8, 's_green'), @(19, 9, 'o_yellow')
)

foreach ($item in $stack)
{
    $mockG.DrawImage($blockBitmaps[$item[2]],
        ($boardX + [int]$item[1] * $PieceSize),
        ($boardY + [int]$item[0] * $PieceSize),
        $PieceSize, $PieceSize)
}

# Peca T a cair e o respectivo fantasma
$ghostT = New-GhostBitmap -Base (ConvertTo-Color $Palette['t_purple']) -Size $PieceSize
$falling = @(@(4, 4), @(4, 5), @(4, 6), @(5, 5))
foreach ($item in $falling)
{
    $mockG.DrawImage($blockBitmaps['t_purple'],
        ($boardX + $item[1] * $PieceSize), ($boardY + $item[0] * $PieceSize), $PieceSize, $PieceSize)
    $mockG.DrawImage($ghostT,
        ($boardX + $item[1] * $PieceSize), ($boardY + ($item[0] + 11) * $PieceSize), $PieceSize, $PieceSize)
}
$ghostT.Dispose()

# Proximas pecas dentro do painel
$smallI = New-BlockBitmap -Base (ConvertTo-Color $Palette['i_cyan'])   -Size $NextPieceSize
$smallL = New-BlockBitmap -Base (ConvertTo-Color $Palette['l_orange']) -Size $NextPieceSize
$smallO = New-BlockBitmap -Base (ConvertTo-Color $Palette['o_yellow']) -Size $NextPieceSize

for ($i = 0; $i -lt 4; $i++)
{
    $mockG.DrawImage($smallI, ($nextX + 4 * $NextPieceSize), ($boardY + (1 + $i) * $NextPieceSize), $NextPieceSize, $NextPieceSize)
}
foreach ($item in @(@(0, 4), @(1, 4), @(2, 4), @(2, 5)))
{
    $mockG.DrawImage($smallL, ($nextX + $item[1] * $NextPieceSize), ($boardY + (7 + $item[0]) * $NextPieceSize), $NextPieceSize, $NextPieceSize)
}
foreach ($item in @(@(0, 4), @(0, 5), @(1, 4), @(1, 5)))
{
    $mockG.DrawImage($smallO, ($nextX + $item[1] * $NextPieceSize), ($boardY + (13 + $item[0]) * $NextPieceSize), $NextPieceSize, $NextPieceSize)
}
$smallI.Dispose(); $smallL.Dispose(); $smallO.Dispose()

# Teclas em baixo a esquerda
$keyFiles = @('key_left.png', 'key_right.png', 'key_down.png', 'key_q.png', 'key_e.png', 'key_esc.png')
$keyX = 40
foreach ($file in $keyFiles)
{
    $img = [System.Drawing.Image]::FromFile((Join-Path $keysDir $file))
    $mockG.DrawImage($img, $keyX, 640, $img.Width, $img.Height)
    $keyX += $img.Width + 10
    $img.Dispose()
}

$mockG.Dispose()
Save-Png $mock.Bitmap (Join-Path $previewDir 'screen_mockup.png')
$mock.Bitmap.Dispose()

# Folha de contacto com todos os blocos
$sheetW = 720
$sheetH = 320
$sheet  = New-Canvas -Width $sheetW -Height $sheetH
$sg     = $sheet.Graphics
$sheetBg = New-Object System.Drawing.SolidBrush((ConvertTo-Color $InkBase))
$sg.FillRectangle($sheetBg, 0, 0, $sheetW, $sheetH)
$sheetBg.Dispose()

$labelFont  = Get-ProjectFont -File 'CrimsonText-SemiBold.ttf' -Size 13
$labelBrush = New-Object System.Drawing.SolidBrush((ConvertTo-Color '#93A3B8' 255))
$titleFont  = Get-ProjectFont -File 'CrimsonText-Bold.ttf' -Size 18 -Style ([System.Drawing.FontStyle]::Bold)
$titleBrush = New-Object System.Drawing.SolidBrush((ConvertTo-Color '#E6EDF6' 255))

$sg.DrawString('Blocos 30px / 15px / fantasma', $titleFont, $titleBrush, 24, 18)

$col = 0
foreach ($name in $Palette.Keys)
{
    $x = 24 + ($col * 98)
    $sg.DrawImage($blockBitmaps[$name], $x, 60, $PieceSize, $PieceSize)

    $small = New-BlockBitmap -Base (ConvertTo-Color $Palette[$name]) -Size $NextPieceSize
    $sg.DrawImage($small, ($x + 38), 60, $NextPieceSize, $NextPieceSize)
    $small.Dispose()

    $ghost = New-GhostBitmap -Base (ConvertTo-Color $Palette[$name]) -Size $PieceSize
    $sg.DrawImage($ghost, ($x + 58), 60, $PieceSize, $PieceSize)
    $ghost.Dispose()

    $sg.DrawString($name, $labelFont, $labelBrush, ($x - 2), 96)
    $sg.DrawString($Palette[$name].ToUpper(), $labelFont, $labelBrush, ($x - 2), 114)
    $col++
}

$sg.DrawString('Logo, paineis e teclas', $titleFont, $titleBrush, 24, 150)
$sg.DrawImage([System.Drawing.Image]::FromFile((Join-Path $imagesRoot 'logo.png')), 24, 176, 305, 90)
$sg.DrawImage([System.Drawing.Image]::FromFile((Join-Path $uiDir 'panel_next.png')), 350, 176, 68, 135)

$keyX = 440
foreach ($file in @('key_left.png', 'key_right.png', 'key_down.png', 'key_esc.png'))
{
    $img = [System.Drawing.Image]::FromFile((Join-Path $keysDir $file))
    $sg.DrawImage($img, $keyX, 200, $img.Width, $img.Height)
    $keyX += $img.Width + 8
    $img.Dispose()
}

$sg.DrawImage([System.Drawing.Image]::FromFile((Join-Path $uiDir 'panel_overlay.png')), 440, 256, 250, 50)

$labelFont.Dispose(); $labelBrush.Dispose(); $titleFont.Dispose(); $titleBrush.Dispose()
$sg.Dispose()
Save-Png $sheet.Bitmap (Join-Path $previewDir 'assets_sheet.png')
$sheet.Bitmap.Dispose()

foreach ($bmp in $blockBitmaps.Values) { $bmp.Dispose() }

Write-Host "`nConcluido." -ForegroundColor Green
