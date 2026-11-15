# tabelog's local website

个人静态小站：纯前端小工具、Googology 大数资源导引、Deductrium 形式系统资料，以及历史版本归档。
无构建步骤、无运行时依赖，可直接由 GitHub Pages 托管；所有工具都在浏览器本地完成计算。

## 目录结构

```
/
├── index.html            站点门户首页
├── 404.html              统一 404 页
├── assets/               全站设计系统
│   ├── site.css          tokens / 顶栏 / 面包屑 / 页脚 / 首页组件（单一设计源）
│   ├── site.js           当前栏目高亮 + 统一页脚注入
│   ├── tools.css         工具页专属外观
│   └── tools-form.css    工具表单控件外观
├── tools/                小工具集合（5 个工具 HTML 路径保持不变）
│   ├── index.html        工具导航
│   ├── *.html / *.js     当前版本工具
│   └── original/         早期版本归档（只读）
├── googology/            Googology 中心页
├── deductrium/           Deductrium 中心页 + HoTT 交互可视化
│   └── walkthrough/      攻略存档旧路径（仅保留一张跳转提示页）
└── OI/                   竞赛笔记（本轮重构忽略，内容保持原样）
```

## 攻略存档已迁至独立站点

攻略存档（原文链接、Markdown 历史存档、各文档中心页、在线查看器、自动生成脚本）已整体搬到独立仓库
[`tabelog-lg/deductrium-walkthrough`](https://github.com/tabelog-lg/deductrium-walkthrough)，
线上地址：**https://tabelog-lg.github.io/deductrium-walkthrough/**。

原站这边保留的内容：

- `deductrium/index.html`「📘 攻略」区块顶部的入口卡片，指向新站（新标签页打开）；下方 13 篇 HedgeDoc 攻略原文链接与其它攻略链接保持原样。
- `deductrium/walkthrough/index.html`：一张**跳转提示页**，2 秒后自动前往新站（`?stay=1` 可留在本页）。这样旧的 `/deductrium/walkthrough/` 收藏不会变成死链。

已从原站移除的内容：

- `deductrium/walkthrough/` 下的 933 份历史存档 `.md`、11 个中心页、2 份静态 `.md`、`articles.json` / `names.json` / 栏目 README；
- `assets/walkthrough.css`（只服务旧攻略页）；
- `scripts/` 整个目录（`gen_walkthrough.py`、`shell_common.py`、`sync-shell.py` 与缓存）——生成脚本已搬去新仓库，原站不再需要；
- `.github/workflows/walkthrough.yml`——自动重建工作流已搬去新仓库。

> 深层旧链接（如 `/deductrium/walkthrough/docs/ch1.html`）已不再可用，会落到统一 404 页；
> 请从新站主页进入对应文档中心页。新站的页面生成、文件名规范化与 push 自动重建，
> 见新仓库的 `README.md` 与 `ARCHIVE-GUIDE.md`。

## 本轮重构要点

1. **补上站点入口**：新增根级 `index.html` 门户与 `404.html`，把原先散落在 `tools/` 里的事实首页升级为真正的全站首页。
2. **栏目拆分**：`googology/` 与 `deductrium/` 从 `tools/` 提升为根级独立栏目；`tools/index.html` 只保留 5 个工具与归档入口，中心页不再混在工具卡片里。
3. **单一设计源**：原 `base.css` / `nav.css` / `tools.css` / `tools-form.css` 四层样式归并为 `assets/site.css`（含兼容旧类名的样式层）+ `assets/tools.css` + `assets/tools-form.css`，所有页面统一引用。
4. **全站壳层统一**：顶栏、面包屑、页脚在所有内容页一致；`site.js` 按页面路径高亮当前栏目并向容器末尾注入统一页脚。顶栏与面包屑是静态 HTML，无 JS 时导航依然可用。
5. **保留项**：`tools/` 下 5 个工具 HTML 的位置与功能逻辑不变；`tools/original/` 历史归档保持原样；`OI/` 目录本轮完全不动，仅在首页以「整理中」占位说明。

## 约定

- 工具页面地址（如 `tools/ddtparse.html`）固定，不做重定向或改名，已有收藏链接继续有效。
- 内容页的 `<head>` 只需引入 `assets/site.css` 与 `assets/site.js`（按目录层级写相对路径）；工具页额外引入 `assets/tools.css`、`assets/tools-form.css`。
- `<body>` 上的 `data-section`（`home` / `tools` / `googology` / `deductrium` / `archive`）决定顶栏高亮项。
- HoTT 可视化页是全屏交互演示，不引入全站样式，仅保留一个浮动返回链接。
- 顶栏、面包屑与页脚已静态写入各 HTML；`site.js` 只负责高亮与在缺少页脚时补一个，站点无构建步骤、无生成脚本依赖。
- 攻略存档的维护（新增 `.md`、自动改名、生成中心页）全部在独立仓库 `deductrium-walkthrough` 进行，原站只保留入口链接与跳转提示页。

## 本地预览

```bash
python3 -m http.server 8000
# 打开 http://localhost:8000/
```

## 部署

推送到 `main` 分支即可由 GitHub Pages 托管（仓库名为 `tabelog-lg.github.io`）。

## 来源

基于 [tabelog-lg/tabelog-lg.github.io](https://github.com/tabelog-lg/tabelog-lg.github.io) 的 `main` 分支快照重构。
