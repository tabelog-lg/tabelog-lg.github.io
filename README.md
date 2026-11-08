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
│   ├── tools-form.css    工具表单控件外观
│   └── walkthrough.css   攻略存档栏目样式
├── tools/                小工具集合（5 个工具 HTML 路径保持不变）
│   ├── index.html        工具导航
│   ├── *.html / *.js     当前版本工具
│   └── original/         早期版本归档（只读）
├── googology/            Googology 中心页
├── deductrium/           Deductrium 中心页 + HoTT 交互可视化
│   └── walkthrough/      攻略存档：names.json 代号表 / archives / static / docs（页面自动生成）
├── scripts/              站点生成脚本（共用壳层模板）
├── .github/workflows/    push 时自动重建攻略存档页面
└── OI/                   竞赛笔记（本轮重构忽略，内容保持原样）
```

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
- 顶栏与面包屑由 `scripts/shell_common.py` 提供模板，`scripts/sync-shell.py` 统一写入各页（脚本幂等）。改导航项后运行 `python3 scripts/sync-shell.py`；历史归档快照与 HoTT 可视化页不在同步范围内。
- **文件名全部 ASCII**：仓库内所有文件名（含生成的页面、存档 `.md`）都是 ASCII，中文只出现在文件内容里。`deductrium/walkthrough/names.json` 是「中文文档名 → 短代号」的对照表。
- **攻略存档**：`scripts/gen_walkthrough.py` 运行时会先按 `names.json` 把 `archives/` 与 `static/` 下新出现的中文名 `.md` 批量改成短代号名（幂等；改代号、新增文档、替换文件都会被增量识别），再生成主页与各中心页。`static/` 下两个静态 `.md` 不参与历史记录，只在主页保留链接。运行 `python3 scripts/gen_walkthrough.py` 或由 `.github/workflows/walkthrough.yml` 在 push 时自动重建并把改名一起提交。规则见 `deductrium/walkthrough/README.md`。

## 本地预览

```bash
python3 -m http.server 8000
# 打开 http://localhost:8000/
```

## 部署

推送到 `main` 分支即可由 GitHub Pages 托管（仓库名为 `tabelog-lg.github.io`）。

## 来源

基于 [tabelog-lg/tabelog-lg.github.io](https://github.com/tabelog-lg/tabelog-lg.github.io) 的 `main` 分支快照重构。
