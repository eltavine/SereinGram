# Linux 发行版

SereinGram 支持讨论度最高的七个 Linux 发行版：Ubuntu、Debian、Linux Mint、Fedora、openSUSE、Arch Linux 与 NixOS，衍生版沿用其上游发行版的安装方式。下载地址与文件名见[发布约定](releases.md)。

| 发行版 | 安装方式 | CI 验证 | 同样适用 |
| --- | --- | --- | --- |
| Ubuntu | `.deb` | 22.04、24.04、26.04 中用 apt 安装 | Pop!_OS、elementary OS、Zorin OS、KDE neon |
| Debian | `.deb` | 12、13 中用 apt 安装 | MX Linux、LMDE |
| Linux Mint | `.deb` | 22.3 中用 apt 安装（x86_64） | |
| Fedora | `.rpm` | 43 中用 dnf 安装 | Nobara；RHEL、Rocky Linux、AlmaLinux 8 起 |
| openSUSE | `.rpm` | Tumbleweed、Leap 16.1 中用 zypper 安装 | |
| Arch Linux | `packaging/arch/PKGBUILD`（系统库）；便携版、AppImage | PKGBUILD 构建并安装；便携版在 Arch 容器中检查动态库（x86_64） | Manjaro、EndeavourOS、CachyOS |
| NixOS | `flake.nix`（基于 nixpkgs 的 telegram-desktop 配方） | x86_64 与 aarch64 从源码构建并启动 | 任何装有 Nix 的发行版 |

`.deb` 与 `.rpm` 的安装测试由 `tools/serein/linux_package_test.sh` 完成：在各发行版的容器中用系统包管理器安装，解析依赖后检查程序、桌面入口、AppStream 元数据、图标，以及程序需要的动态库都能找到。Linux 工作流对 x86_64 与 arm64 的每次构建都运行它。

所有发行版也都可以使用 Flatpak 单文件包、AppImage 或便携版压缩包（glibc 2.28 起）。支持 Snap 的发行版还可以安装 Snap 包：`snap/snapcraft.yaml` 以 core24 构建，CI 工作流 `serein-snap.yml` 在配方改动时和每周构建、安装并启动它。

## 安装

产物不签名，下载后可先按[发布约定](releases.md#3-校验文件)用 `SHA256SUMS` 校验。

Ubuntu、Debian、Linux Mint：

```sh
sudo apt install ./SereinGram-linux-x86_64.deb
```

Fedora：

```sh
sudo dnf install ./SereinGram-linux-x86_64.rpm
```

openSUSE：

```sh
sudo zypper install --allow-unsigned-rpm ./SereinGram-linux-x86_64.rpm
```

arm64 设备把文件名中的 `x86_64` 换成 `arm64`。

Arch Linux 可以直接使用 AppImage 或便携版，也可以用系统库从源码构建：

```sh
git clone https://github.com/eltavine/SereinGram.git
cd SereinGram/packaging/arch
SEREIN_API_ID=<your api_id> SEREIN_API_HASH=<your api_hash> makepkg -si
```

## Snap

Snap 包尚未上架 Snap Store，下载或自行构建后用 `--dangerous` 安装本地文件：

```sh
sudo snap install --dangerous ./SereinGram-linux-x86_64.snap
```

配方不含任何 API 凭据。自行构建时，在 `snap/snapcraft.yaml` 的 `cmake-parameters` 中加入自己的 `-DTDESKTOP_API_ID` 与 `-DTDESKTOP_API_HASH`（不要提交这项改动）；CI 只在构建用的副本中加入。

## NixOS 与 Nix

`flake.nix` 导出 `packages.<系统>.sereingram`（也是 `default`）与 `overlays.default`，支持 `x86_64-linux` 与 `aarch64-linux`。配方 `packaging/nix/package.nix` 在 nixpkgs 的 telegram-desktop 之上替换源码，依赖直接取自 nixpkgs 的二进制缓存，SereinGram 本身从源码编译。flake 需要构建子模块，要求 Nix 2.27 或更新版本。

SereinGram 不使用 Telegram 官方客户端的凭据，也不把自己的发布凭据写进仓库，所以 Nix 包与 PKGBUILD 一样需要你提供在 <https://my.telegram.org> 申请的 `api_id` 与 `api_hash`，否则构建时报错。在 NixOS 配置中：

```nix
{
  inputs.sereingram.url = "github:eltavine/SereinGram";

  outputs =
    { nixpkgs, sereingram, ... }:
    {
      nixosConfigurations.example = nixpkgs.lib.nixosSystem {
        modules = [
          (
            { pkgs, ... }:
            {
              environment.systemPackages = [
                (sereingram.packages.${pkgs.stdenv.hostPlatform.system}.default.override {
                  apiId = 12345;
                  apiHash = "your api_hash";
                })
              ];
            }
          )
        ];
      };
    };
}
```

也可以加入 `sereingram.overlays.default`，再使用 `pkgs.sereingram.override { ... }`。`testCredentials = true` 改用上游的测试凭据，只用于自动化检查。

Nix 构建以系统库运行，不检查 GitHub 更新，由 `nix flake update` 更新。
