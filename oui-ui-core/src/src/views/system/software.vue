<template>
  <div>
    <el-card style=" margin-bottom: 10px">
      <el-form label-width="200px" style="max-width: 600px">
        <el-form-item :label="$t('Used space')">
          <el-progress :text-inside="true" :stroke-width="20" :percentage="diskUsedPercentage"></el-progress>
          <span>{{ diskUsedDetailed }}</span>
        </el-form-item>
        <el-form-item :label="$t('Update package lists')">
          <el-button type="primary" size="small" @click="updatePackage">{{ $t('Start update...') }}</el-button>
        </el-form-item>
        <el-form-item :label="$t('Install package directly')">
          <el-row>
            <el-col :span="20"><el-input v-model="packageName" :placeholder="$t('Enter URL or name...')"></el-input></el-col>
            <el-col :span="4"><el-button type="primary" @click="installRemovePackage(packageName, false)">>></el-button></el-col>
          </el-row>
        </el-form-item>
        <el-form-item :label="$t('Filter packages')">
          <el-input v-model="filter" :placeholder="$t('Filter packages...')" @keyup.enter.native="fetchPackageList(0)"></el-input>
          <el-checkbox v-model="displayInstalled">{{ $t('Display only installed packages') }}</el-checkbox>
        </el-form-item>
      </el-form>
    </el-card>
    <el-card>
      <div slot="header">
        <el-pagination background layout="prev, pager, next" :page-size="limit" :total="total" :current-page.sync="currentPage"></el-pagination>
      </div>
      <el-table :data="packages" v-loading="loading" :element-loading-text="$t('Loading...')"  element-loading-spinner="el-icon-loading" element-loading-background="rgba(0, 0, 0, 0.8)">
        <el-table-column prop="name" :label="$t('Software-Package')" width="200"></el-table-column>
        <el-table-column prop="version" :label="$t('Version')" width="200" show-overflow-tooltip></el-table-column>
        <el-table-column prop="size" :label="$t('Size') + '(KB)'" width="100"></el-table-column>
        <el-table-column prop="description" :label="$t('Description')" show-overflow-tooltip>
        </el-table-column>
        <el-table-column label="#" width="200">
          <template v-slot="{ row }">
            <div style="display: flex;">
              <el-button :type="row.installed ? 'success' : 'danger'" size="mini" @click="installRemovePackage(row.name, row.installed)">{{ row.installed ? $t('Installed') : $t('Not installed') }}</el-button>
              <el-tooltip v-if="row.new_version" placement="top" :content="$t('Can be upgrade to', {ver: row.new_version})">
                <el-button type="primary" size="mini" @click="installRemovePackage(row.name, row.installed, row.new_version)">{{ $t('Upgradable') }}</el-button>
              </el-tooltip>
            </div>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script>
export default {
  data() {
    return {
      diskInfo: {
        used: 0,
        total: 0
      },
      diskSpace: 0,
      packageName: '',
      filter: '',
      packages: [],
      total: 0,
      limit: 50,
      currentPage: 1,
      displayInstalled: false,
      installedList: {},
      loading: true
    }
  },
  computed: {
    diskUsedPercentage() {
      if (this.diskInfo.used === 0)
        return 0;
      return parseInt(this.diskInfo.used / this.diskInfo.total * 100);
    },
    diskUsedDetailed() {
      const used = parseInt(this.diskInfo.used / 1024) + ' KB';
      const total = parseInt(this.diskInfo.total / 1024) + ' KB';
      return `${used} / ${total} ` + this.$t('used');
    }
  },
  methods: {
    doFetchPackageList(cmd, offset, limit, pattern) {
      return new Promise((resolve, reject) => {
        this.$ubus.call('oui.opkg', cmd, {offset, limit, pattern}).then(r1 => {
          const total = r1.total;
          const packages = r1.packages;

          if (typeof(offset) === 'undefined')
            offset = 0;

          if (typeof(limit) === 'undefined' || limit === 0 || limit > total)
            limit = total;

          if (packages.length < limit) {
            const batch = [];

            for (let i = offset + packages.length; i < limit; i += 100)
              batch.push(['oui.opkg', cmd, {
                offset: i,
                limit: (Math.min(i + 100, limit) % 100) || 100,
                pattern
              }]);

            this.$ubus.callBatch(batch).then(r2 => {
              if (!Array.isArray(r2))
                r2 = [r2];

              r2.forEach(item => {
                r1.packages.push(...item.packages);
              });

              resolve(r1);
            }).catch(() => {
              reject();
            });
          } else {
            resolve(r1);
          }
        }).catch(() => {
          reject();
        });
      });
    },
    fetchPackageList(offset) {
      let cmd = 'list';
      let pattern = '';

      if (this.filter.length > 0)
        pattern = `*${this.filter}*`;

      if (this.displayInstalled)
        cmd = 'list_installed'

      this.loading = true;
      this.packages = [];

      this.doFetchPackageList(cmd, offset, this.limit, pattern).then(r => {
        this.total = r.total;
        this.packages = r.packages.map(pkg => {
          return {
            ...pkg,
            installed: this.installedList[pkg.name],
            new_version: this.installedList[pkg.name] ? this.installedList[pkg.name].new_version : undefined
          }
        });
        this.loading = false;
      });
    },
    doInstallRemovePackage(name, cmd) {
      return this.$ubus.call('oui.opkg', cmd, {package: name});
    },
    showStatus(res, title) {
      let msg = '';

      if (res.stdout)
        msg += `<pre style="background-color: #f5f5f5; color: #333;">${res.stdout}</pre>`;

      if (res.stderr)
        msg += `<pre style="background-color: #f5f5f5; color: #F56C6C">${res.stderr}</pre>`;

      if (res.code)
        msg += this.$t('Package manager failed with status', {code: res.code});
      else
        msg += this.$t('Package manager finished successfully.');

      this.$confirm(msg, title, {
        dangerouslyUseHTMLString: true,
        showCancelButton: false,
        confirmButtonText: this.$t('Close')
      }).then(() => {
        if (!res.code) {
          this.loading = true;
          this.updateInstalledList().then(() => {
            this.fetchPackageList((this.currentPage - 1) * this.limit);
          });
        }
      });
    },
    installRemovePackage(name, installed, new_version) {
      if (name === '')
        return;

      let title = installed ? 'Removing package' : 'Installing package';
      let msg = installed ? 'Really remove package' : 'Really install package';
      let cmd = installed ? 'remove' : 'install';

      if (new_version) {
        title = 'Upgrading package';
        msg = 'Really upgrade package';
        cmd = 'upgrade';
        msg = this.$t(msg, {name: name, ver: new_version})
      } else {
        msg = this.$t(msg, {name: name})
      }

      title = this.$t(title, {name: name})

      this.$confirm(msg, title, {
        type: 'info'
      }).then(() => {
        const loading = this.$getLoading(this.$t('Waiting for package manager...'));
        this.doInstallRemovePackage(name, cmd).then(r => {
          loading.close();
          this.showStatus(r, title);
        });
      });
    },
    doUpdatePackage() {
      return this.$ubus.call('oui.opkg', 'update');
    },
    updatePackage() {
      const loading = this.$getLoading(this.$t('Waiting for package manager...'));
      this.doUpdatePackage().then(r => {
        loading.close();
        this.showStatus(r, 'Updating package lists');
      });
    },
    updateInstalledList() {
      return new Promise(resolve => {
        this.doFetchPackageList('list_installed').then(r => {
          this.installedList = {};
          r.packages.forEach(pkg => {
            this.installedList[pkg.name] = {};
          });

          this.doFetchPackageList('list_upgradable').then(r => {
            r.packages.forEach(pkg => {
              this.installedList[pkg.name].new_version = pkg.new_version;
            });
            resolve();
          }).catch(() => {
            resolve();
          });
        });
      });
    }
  },
  watch: {
    displayInstalled() {
      this.fetchPackageList(0);
    },
    currentPage() {
      this.fetchPackageList((this.currentPage - 1) * this.limit);
    }
  },
  created() {
    this.$system.getDiskInfo().then(info => {
      this.diskInfo.used = info.root.used;
      this.diskInfo.total = info.root.total;
    });

    this.updateInstalledList().then(() => {
      this.fetchPackageList(0);
    });
  }
}
</script>
