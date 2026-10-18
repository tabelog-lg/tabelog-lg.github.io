import os
import json
import shutil
import subprocess

def process_folder(main_folder):
    # 遍历主文件夹下的每个 id_folder（如 114343823806631）
    for id_dir in os.listdir(main_folder):
        id_path = os.path.join(main_folder, id_dir)
        if not os.path.isdir(id_path):
            continue

        # 遍历 id_folder 下的每个 c_folder（如 c_29437070528）
        for c_dir in os.listdir(id_path):
            c_path = os.path.join(id_path, c_dir)
            if not os.path.isdir(c_path):
                continue

            # --- 读取 entry.json 获取标题 ---
            entry_json_path = os.path.join(c_path, 'entry.json')
            video_title = None
            if os.path.isfile(entry_json_path):
                try:
                    with open(entry_json_path, 'r', encoding='utf-8') as f:
                        data = json.load(f)
                        video_title = data.get('title')
                except Exception as e:
                    print(f"读取 {entry_json_path} 出错: {e}")

            # --- 检查封面图片 ---
            cover_path = os.path.join(c_path, 'cover.jpg')
            has_cover = os.path.isfile(cover_path)

            # 遍历 c_folder 下的每个 num_folder（如 80）
            for num_dir in os.listdir(c_path):
                num_path = os.path.join(c_path, num_dir)
                if not os.path.isdir(num_path):
                    continue

                video_path = os.path.join(num_path, 'video.m4s')
                audio_path = os.path.join(num_path, 'audio.m4s')
                if not (os.path.isfile(video_path) and os.path.isfile(audio_path)):
                    print(f"跳过 {num_path}，缺少音视频文件")
                    continue

                # --- 构建 ffmpeg 命令 ---
                # 基础输入：视频、音频
                cmd = ['ffmpeg']
                cmd.extend(['-i', 'video.m4s'])
                cmd.extend(['-i', 'audio.m4s'])

                # 如果有封面，添加封面输入
                if has_cover:
                    # 注意：封面文件路径相对于工作目录（num_path）是 '../cover.jpg'
                    cmd.extend(['-i', '../cover.jpg'])

                # 输出选项
                cmd.extend(['-c:v', 'copy', '-c:a', 'aac', '-strict', 'experimental'])

                # 如果有封面，设置附加封面（attached_pic）并映射流
                if has_cover:
                    # 流映射：0:v 视频，0:a 音频，1 封面图片（索引2因为有两个输入？实际索引：0=video.m4s, 1=audio.m4s, 2=cover.jpg）
                    # 注意：ffmpeg 输入索引按命令行顺序：0->video.m4s, 1->audio.m4s, 2->cover.jpg
                    # 视频流：0:v (来自第一个输入)，音频流：1:a (来自第二个输入)，封面：2:v (来自第三个输入)
                    cmd.extend([
                        '-map', '0:v',
                        '-map', '1:a',
                        '-map', '2:v',
                        '-disposition:v:1', 'attached_pic'   # 将第二个视频流（封面）标记为附加封面
                    ])
                else:
                    # 无封面：正常映射
                    cmd.extend(['-map', '0:v', '-map', '1:a'])

                # 设置视频标题元数据（如果有）
                if video_title:
                    cmd.extend(['-metadata', f'title={video_title}'])

                cmd.append('output.mp4')

                # --- 执行 ffmpeg ---
                try:
                    subprocess.run(cmd, cwd=num_path, check=True)
                except subprocess.CalledProcessError as e:
                    print(f"处理 {num_path} 失败，ffmpeg错误: {e}")
                    continue
                except Exception as e:
                    print(f"处理 {num_path} 时发生意外错误: {e}")
                    continue

                # --- 移动输出文件到主文件夹 ---
                output_filename = f"{c_dir}_{num_dir}.mp4"
                output_src = os.path.join(num_path, 'output.mp4')
                output_dst = os.path.join(main_folder, output_filename)

                try:
                    if os.path.exists(output_dst):
                        os.remove(output_dst)
                    shutil.move(output_src, output_dst)
                except Exception as e:
                    print(f"移动文件失败: {e}")
                    continue

                # --- 删除已处理的 num_folder ---
                try:
                    shutil.rmtree(num_path)
                    print(f"成功处理: {output_filename}" + (f" (标题: {video_title})" if video_title else ""))
                except Exception as e:
                    print(f"删除目录 {num_path} 失败: {e}")

def main_ffmpeg_work():

    main_folder = input("请输入主文件夹路径: ").strip()
    
    if not os.path.isdir(main_folder):
        print("错误：输入路径不是一个有效的目录")
    else:
        process_folder(main_folder)
    print("处理完成")
    os.system('pause')

if __name__ == "__main__":
    main_ffmpeg_work()

"""

def process_folder(main_folder):

    for subdir_name in os.listdir(main_folder):
        subdir_path = os.path.join(main_folder, subdir_name)
        if not os.path.isdir(subdir_path):
            continue
        video_path = os.path.join(subdir_path, 'video.m4s')
        audio_path = os.path.join(subdir_path, 'audio.m4s')
        if not (os.path.isfile(video_path) and os.path.isfile(audio_path)):
            print(f"跳过 {subdir_name}，缺少音视频文件")
            continue

        cmd = [
            'ffmpeg',
            '-i', 'video.m4s',
            '-i', 'audio.m4s',
            '-c:v', 'copy',
            '-c:a', 'aac',
            '-strict', 'experimental',
            'output.mp4'
        ]

        try:
            subprocess.run(cmd, cwd=subdir_path, check=True)
        except subprocess.CalledProcessError as e:
            print(f"处理 {subdir_name} 失败，ffmpeg错误: {e}")
            continue
        except Exception as e:
            print(f"处理 {subdir_name} 时发生意外错误: {e}")
            continue

        output_src = os.path.join(subdir_path, 'output.mp4')
        output_dst = os.path.join(main_folder, f"{subdir_name}.mp4")

        try:
            if os.path.exists(output_dst):
                os.remove(output_dst)
            shutil.move(output_src, output_dst)
        except Exception as e:
            print(f"移动文件失败: {e}")
            continue

        try:
            shutil.rmtree(subdir_path)
            print(f"成功处理: {subdir_name}")
        except Exception as e:
            print(f"删除目录失败: {e}")

def main_ffmpeg_work():

    main_folder = input("请输入主文件夹路径: ").strip()
    
    if not os.path.isdir(main_folder):
        print("错误：输入路径不是一个有效的目录")
    else:
        process_folder(main_folder)
    print("处理完成")
    os.system('pause')

"""

"""

def process_folder(main_folder):
    # 遍历主文件夹下的每个 id_folder（如 114343823806631）
    for id_dir in os.listdir(main_folder):
        id_path = os.path.join(main_folder, id_dir)
        if not os.path.isdir(id_path):
            continue

        # 遍历 id_folder 下的每个 c_folder（如 c_29437070528）
        for c_dir in os.listdir(id_path):
            c_path = os.path.join(id_path, c_dir)
            if not os.path.isdir(c_path):
                continue

            # 遍历 c_folder 下的每个 num_folder（如 80）
            for num_dir in os.listdir(c_path):
                num_path = os.path.join(c_path, num_dir)
                if not os.path.isdir(num_path):
                    continue

                video_path = os.path.join(num_path, 'video.m4s')
                audio_path = os.path.join(num_path, 'audio.m4s')
                if not (os.path.isfile(video_path) and os.path.isfile(audio_path)):
                    print(f"跳过 {num_path}，缺少音视频文件")
                    continue

                # 执行 ffmpeg 合并
                cmd = [
                    'ffmpeg',
                    '-i', 'video.m4s',
                    '-i', 'audio.m4s',
                    '-c:v', 'copy',
                    '-c:a', 'aac',
                    '-strict', 'experimental',
                    'output.mp4'
                ]
                try:
                    subprocess.run(cmd, cwd=num_path, check=True)
                except subprocess.CalledProcessError as e:
                    print(f"处理 {num_path} 失败，ffmpeg错误: {e}")
                    continue
                except Exception as e:
                    print(f"处理 {num_path} 时发生意外错误: {e}")
                    continue

                # 构造输出文件名（使用 c_folder 名称 + 数字子目录名，防止重名）
                output_filename = f"{c_dir}_{num_dir}.mp4"
                output_src = os.path.join(num_path, 'output.mp4')
                output_dst = os.path.join(main_folder, output_filename)

                try:
                    if os.path.exists(output_dst):
                        os.remove(output_dst)
                    shutil.move(output_src, output_dst)
                except Exception as e:
                    print(f"移动文件失败: {e}")
                    continue

                # 删除已处理的 num_folder（保留上级目录）
                try:
                    shutil.rmtree(num_path)
                    print(f"成功处理: {output_filename}")
                except Exception as e:
                    print(f"删除目录 {num_path} 失败: {e}")

"""
