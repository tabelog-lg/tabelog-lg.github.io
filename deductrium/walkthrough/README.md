# 攻略存档 · Deductrium

攻略文章的原文链接、Markdown 历史存档，以及每篇文章的中心页。
主页与各中心页**全部由脚本生成**，新增存档后不用手工改任何 HTML。

> **仓库内所有文件名都是 ASCII。** 中文只出现在文件内容（HTML 正文、JSON 配置、Markdown 正文）里。
> 每次运行 `gen_walkthrough.py` 都会把新放进来的中文名文件批量改成短 ASCII 代号。

## 目录

```
deductrium/walkthrough/
├── index.html        攻略存档主页（生成）
├── names.json        ★ 中文文档名 → ASCII 文件代号（文件名规范的单一来源）
├── articles.json     文章配置：显示名 / 原文网页链接 / 说明 / 静态文档
├── archives/         [代号]_[13位毫秒时间戳].md   ← 参与历史记录
├── static/           [代号].md                    ← 静态文档，不参与历史记录
├── docs/             [代号].html（生成，一篇一个中心页）
└── README.md         本文件
```

`deductrium/index.html` 的「📘 攻略」区块里有通向本页的入口。

## 文件名规范：批量改名

运行

```bash
python3 scripts/gen_walkthrough.py
```

脚本第一件事就是扫描 `archives/` 与 `static/` 下的 `.md`，按 `names.json` 把文件重命名：

| 放进来的文件名 | 自动改成 |
| --- | --- |
| `Deductrium 攻略第一章 命题逻辑_1784452200000.md` | `ch1_1784452200000.md` |
| `附录_1788248400000.md` | `appendix_1788248400000.md` |
| `Fin 自等价数与阶乘.md` | `fin.md` |
| `Deductrium 当前版本 94_96 全可达成就从零复现路线.md` | `route94.md` |

生成的中心页也用代号命名：`docs/ch1.html`、`docs/appendix.html`。

**"有更新就识别那些再改"**：脚本幂等，每次运行都重新扫描，所以下面几种更新都会被自动识别，不需要额外操作：

- 又放进一批中文名存档（同一篇的新时间戳）→ 改名并追加到该篇历史；
- 放进一个 `names.json` / `articles.json` 里都没有的全新中文文档名 → 自动分配 `doc-<hash8>` 代号、写回 `names.json`、生成对应中心页；
- 静态 `.md` 被换回中文名 → 自动改回代号名；
- **修改了 `names.json` 里的代号**（例如 `ch1` 改成 `sec1`）→ 旧代号的文件会批量迁移到新代号，中心页同步改名，旧页面自动清理。

`--check` 只报告待改名 / 待生成的文件，**不改名、不写文件**：

```bash
python3 scripts/gen_walkthrough.py --check
```

改名冲突时（目标文件已存在且内容不同）脚本会跳过并给出警告，不会覆盖任何文件；内容完全相同的重复文件会被自动删除。

## 代号对照表

`names.json` 的 `archives`（历史文档）与 `static`（静态文档）两张表：

| 中文文档名（`slug`） | 代号（`code`） | 用途 |
| --- | --- | --- |
| `Deductrium 攻略第一章 命题逻辑` | `ch1` | 历史存档 |
| `Deductrium 攻略第二章 一阶逻辑` | `ch2` | 历史存档 |
| `Deductrium 攻略第三章 皮亚诺算数(PA)` | `ch3` | 历史存档 |
| `Deductrium 攻略第四章 ZFC(策梅洛-弗兰克尔集合论加选择公理)` | `ch4` | 历史存档 |
| `Deductrium 攻略第五章 数系` | `ch5` | 历史存档 |
| `Deductrium 攻略第六章 类型论Σ依赖和前后` | `ch6` | 历史存档 |
| `Deductrium 攻略第七章 类型论W良基树前后` | `ch7` | 历史存档 |
| `Deductrium 攻略第八章 同伦类型论(第1部分)` | `ch8a` | 历史存档 |
| `Deductrium 攻略第八章 同伦类型论(第2部分)` | `ch8b` | 历史存档 |
| `附录` | `appendix` | 历史存档 |
| `Fin 自等价数与阶乘` | `fin` | 静态文档 |
| `Deductrium 当前版本 94_96 全可达成就从零复现路线` | `route94` | 静态文档 |

> `slug` 是**逻辑标识**（存在 JSON 里、用于分组和显示），不再是文件名前缀；文件名前缀是 `code`。
> 想换更合心意的短名，直接编辑 `names.json` 的 value 再重跑脚本即可。

## 主页布局

- **上半部分**：攻略文章总览表 —— 文章名、原文网页链接、最新存档（时间 + 字符数 + 下载）、存档份数、中心页入口；表格里的小字显示 `archives/<代号>_*.md`。紧接一个「📎 静态文档」小节，列出两个静态 `.md` 的原文链接与查看 / 下载链接。
- **下半部分**：各文档中心页卡片；静态文档用虚线卡片直接链接到 `.md`，明确标注「静态文档 · 不参与历史记录」。

## 中心页内容

每篇历史文档一个页面，按修改时间倒序列出全部历史记录：

| 列 | 口径 |
| --- | --- |
| 修改时间 | 取文件名末尾的 **13 位毫秒级 Unix 时间戳**，换算为 **UTC+8**，格式如 `2026年7月19日 17:10`（月/日/时不补零，分钟补两位） |
| 文件字符数 | 按 UTF-8 解码后的 **Unicode 字符数**，`len(text)`，含换行与空白字符；`\r\n` 计 2 个字符；UTF-8 BOM 不计入 |
| 文件 | 查看 `.md` 原文 / 下载 |

> 时间完全来自**文件名**，不是文件系统的 mtime —— 这样 git clone 后 mtime 变化也不会影响页面结果，结果可复现。

存档份数超过 100 时，表格里的「N 份」圆框也不会换行：`assets/walkthrough.css` 中 `.wk-pill`、`.col-count`、`.wk-card__foot`、`.wk-meta__item .v` 均设置了 `white-space: nowrap`，存档数列宽留到 104px。

## 文件命名

```
[代号]_[13位毫秒时间戳].md
```

例如 `ch1_1784452200000.md` → `2026年7月19日 17:10`。

- 代号来自 `names.json`，必须全 ASCII；推荐 `[a-z0-9-]`，避免路径分隔符。
- 时间戳必须是恰好 13 位数字；不符合命名或时间戳非法的文件会被跳过并在生成时给出提示。
- 同一代号 + 不同时间戳 = 同一篇文章的多个历史版本。
- 也可以直接放中文名文件（`[中文文档名]_[13位时间戳].md`），脚本会在运行时自动改名；长期来看建议直接用代号名，少一次变更。

生成毫秒时间戳：

```bash
# Python：把 UTC+8 的 2026-07-19 17:10 转成 13 位时间戳
python3 -c "from datetime import datetime,timezone,timedelta; print(int(datetime(2026,7,19,17,10,tzinfo=timezone(timedelta(hours=8))).timestamp()*1000))"
# 或者取当前时间
python3 -c "import time; print(int(time.time()*1000))"
```

## 静态文档（不参与历史记录）

`static/` 下目前是两个**空占位文件**：

```
static/fin.md
static/route94.md
```

它们只有一份 `.md`，没有时间戳，因此不进 `archives/`、不生成中心页、不出现在历史记录统计里；主页仍保留指向这两个文件的「查看 .md / 下载」链接，以及各自的原文网页链接（94/96 路线原文分 (1)(2) 两篇，所以配置里给了两个 url，页面显示「原文 (1)」「原文 (2)」）。

> `route94` 里的数字与原来文件名中的 `94_96` 已无关；静态文件单独放在 `static/`，扫描器只看 `archives/` 里带 13 位时间戳的文件，双重隔离。

往里面写入真实内容、或调整文件名时：把文件放好，同步改 `articles.json` 的 `static` 列表（以及可选的 `names.json`），重新运行生成脚本即可。

## articles.json

```json
{
  "articles": [
    {
      "slug": "Deductrium 攻略第一章 命题逻辑",
      "name": "第一章 命题逻辑",
      "url": "https://hedgedoc.envs.net/IU_LnfotQT2iZjdZId9YFw",
      "note": "命题逻辑入门、宏录制与双重否定"
    }
  ],
  "static": [
    {
      "slug": "Fin 自等价数与阶乘",
      "name": "Fin 自等价数与阶乘",
      "file": "fin.md",
      "urls": ["https://hedgedoc.envs.net/qMcJimEyTuCbkUWFjPBenA"],
      "note": "静态文档，不参与历史记录"
    }
  ]
}
```

| 字段 | 所属 | 说明 |
| --- | --- | --- |
| `slug` | 两者 | 中文文档名，逻辑标识，必须和 `names.json` 的 key 一致 |
| `name` | 两者 | 页面上显示的文章名（可含 `/` 等不适合做文件名的字符） |
| `url` | articles | 攻略原文的在线地址；留空则该列显示「—」 |
| `note` | 两者 | 卡片 / 静态条目上的一句说明，可留空 |
| `file` | static | `static/` 下的 ASCII 文件名 |
| `urls` | static | 原文链接数组，1 个显示「原文网页」，多个显示「原文 (1)(2)…」 |

`articles` 里没写的文档名，只要 `archives/` 里有对应 `.md`，也会被**自动发现**并生成中心页。`static` 列表里的项永远不生成中心页；之前误生成的静态中心页会在下次运行时自动清理。

## 新增 / 删除存档

**本地**：把 `.md` 放进 `archives/`（中文名或代号名都行），或删掉某份，然后：

```bash
python3 scripts/gen_walkthrough.py
```

提交 `archives/`、`static/`、`names.json` 与生成出来的 `index.html`、`docs/` 即可。

**线上**：push 到 `main` 时 `.github/workflows/walkthrough.yml` 会自动运行生成脚本，`git add -A deductrium/walkthrough` 把**改名后的文件、names.json、生成页面一起提交**回仓库，无需本地操作。需要在仓库 Settings → Actions 里允许 workflow 拥有 `contents: write` 权限；若 `main` 有分支保护，请允许 GitHub Actions 推送，或改用本地生成 + 手动提交。

生成脚本是幂等的：会更新已有页面，并清理已删除文章 / 旧代号遗留的孤儿中心页。

## 关于当前附带的存档

`archives/` 里目前是 **13 份演示存档**（已改成 ASCII 代号名），覆盖全部 10 篇文档（其中 `ch1` 有 4 个历史版本，用来展示多版本效果）；
文章配置里的原文链接取自 `deductrium/index.html` 的「📘 攻略」区块。

替换为真实存档时，直接删掉 `archives/` 下全部演示 `.md`，放入自己的
`[代号]_[13位时间戳].md`（或中文名文件让脚本自动改），按需调整 `articles.json` / `names.json`，重新运行生成脚本即可。

## 关于 `.nojekyll`

仓库根目录放了 `.nojekyll`，让 GitHub Pages 跳过 Jekyll、按纯静态文件发布，
避免 `.md` 存档被 Jekyll 当成页面源处理。"查看 .md" 链接能否在浏览器内直接显示取决于
浏览器对 Markdown 文件 MIME 类型的处理；需要保存时用旁边的「下载 ↓」。

## 相关文件

- `scripts/gen_walkthrough.py` —— 文件名规范化 + 生成逻辑与 HTML 模板
- `scripts/shell_common.py` —— 全站顶栏 / 面包屑 / 页脚模板（与 `sync-shell.py` 共用）
- `assets/walkthrough.css` —— 本栏目样式
