<template>
  <div>
    <a-card style="margin-bottom: 10px">
      <a-form-model :label-col="labelCol" :wrapper-col="wrapperCol">
        <a-form-model-item :label="$t('Used space')">
          <a-progress :percent="diskUsedPercentage"/>
          <span>{{ diskUsedDetailed }}</span>
        </a-form-model-item>
        <a-form-model-item :label="$t('Update package lists')">
          <a-button type="primary" size="small" @click="updatePackage">{{ $t('Start update...') }}</a-button>
        </a-form-model-item>
        <a-form-model-item :label="$t('Install package directly')">
          <a-input v-model="packageName" :placeholder="$t('Enter URL or name...')" search enter-button=">>" @on-search="installRemovePackage(packageName, false)"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('Filter packages')">
          <a-input v-model="filter" :placeholder="$t('Filter packages...')" @keyup.enter.native="fetchPackageList(0)"/>
          <a-checkbox v-model="displayInstalled">{{ $t('Display only installed packages') }}</a-checkbox>
        </a-form-model-item>
      </a-form-model>
    </a-card>
    <a-card>
      <template #title>
        <a-pagination :page-size="limit" :total="total" v-model="currentPage"/>
      </template>
      <a-table :columns="columns" :data-source="dataSource" :pagination="false" :loading="loading">
        <template #operate="record">
          <a-button :type="record.installed ? 'primary' : ''" size="small" @click="installRemovePackage(record.name, record.installed)">{{ record.installed ? $t('Installed') : $t('Not installed') }}</a-button>
          <a-tooltip v-if="record.new_version" :title="$t('Can be upgrade to', {ver: record.new_version})" style="margin-left: 10px">
            <a-button type="primary" size="small" @click="installRemovePackage(record.name, record.installed, record.new_version)">{{ $t('Upgradable') }}</a-button>
          </a-tooltip>
        </template>
      </a-table>
    </a-card>
  </div>
</template>

<script>
export default {
  data () {
    return {
      labelCol: { span: 4 },
      wrapperCol: { span: 14 },
      diskInfo: {
        used: 0,
        total: 0
      },
      diskSpace: 0,
      packageName: '',
      filter: '',
      columns: [
        { dataIndex: 'name', title: this.$t('Software-Package') },
        { dataIndex: 'version', title: this.$t('Version') },
        { dataIndex: 'size', title: this.$t('Size') + '(KB)' },
        { dataIndex: 'description', title: this.$t('Description') },
        { key: 'operate', title: '#', width: 200, scopedSlots: { customRender: 'operate' } }
      ],
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
    diskUsedPercentage () {
      if (this.diskInfo.used === 0) { return 0 }
      return Math.floor(this.diskInfo.used / this.diskInfo.total * 100)
    },
    diskUsedDetailed () {
      const used = Math.floor(this.diskInfo.used / 1024) + ' KB'
      const total = Math.floor(this.diskInfo.total / 1024) + ' KB'
      return `${used} / ${total} ` + this.$t('used')
    },
    dataSource () {
      return this.packages.map((p, i) => {
        p.key = i
        return p
      })
    }
  },
  methods: {
    doFetchPackageList (cmd, offset, limit, pattern) {
      return new Promise((resolve, reject) => {
        this.$ubus.call('oui.opkg', cmd, { offset, limit, pattern }).then(r1 => {
          const total = r1.total
          const packages = r1.packages

          if (typeof (offset) === 'undefined') { offset = 0 }

          if (typeof (limit) === 'undefined' || limit === 0 || limit > total) { limit = total }

          if (packages.length < limit) {
            const batch = []

            for (let i = offset + packages.length; i < limit; i += 100) {
              batch.push(['oui.opkg', cmd, {
                offset: i,
                limit: (Math.min(i + 100, limit) % 100) || 100,
                pattern
              }])
            }

            this.$ubus.callBatch(batch).then(r2 => {
              if (!Array.isArray(r2)) { r2 = [r2] }

              r2.forEach(item => {
                packages.push(...item.packages)
              })

              resolve(r1)
            }).catch(() => {
              reject(new Error(''))
            })
          } else {
            resolve(r1)
          }
        }).catch(() => {
          reject(new Error(''))
        })
      })
    },
    fetchPackageList (offset) {
      let cmd = 'list'
      let pattern = ''

      if (this.filter.length > 0) { pattern = `*${this.filter}*` }

      if (this.displayInstalled) { cmd = 'list_installed' }

      this.loading = true
      this.packages = []

      let limit = this.limit
      if (this.total > 0 && offset + limit > this.total) {
        limit = this.total - offset
      }

      this.doFetchPackageList(cmd, offset, limit, pattern).then(r => {
        this.total = r.total
        this.packages = r.packages.map(pkg => {
          return {
            ...pkg,
            installed: this.installedList[pkg.name],
            new_version: this.installedList[pkg.name] ? this.installedList[pkg.name].new_version : undefined
          }
        })
        this.loading = false
      })
    },
    doInstallRemovePackage (name, cmd) {
      return this.$ubus.call('oui.opkg', cmd, { package: name })
    },
    showStatus (res, title) {
      this.$info({
        title,
        content: h => {
          const content = []
          if (res.stdout) {
            content.push(h('pre', {
              style: {
                backgroundColor: '#f5f5f5',
                color: '#333'
              }
            }, [res.stdout]))
          }

          if (res.stderr) {
            content.push(h('pre', {
              style: {
                backgroundColor: '#f5f5f5',
                color: '#F56C6C'
              }
            }, [res.stderr]))
          }

          if (res.code) {
            content.push(h('p', [this.$t('Package manager failed with status', { code: res.code })]))
          } else {
            content.push(h('p', [this.$t('Package manager finished successfully.')]))
          }

          return h('div', content)
        },
        onOk: () => {
          if (!res.code) {
            this.loading = true
            this.updateInstalledList().then(() => {
              this.fetchPackageList((this.currentPage - 1) * this.limit)
            })
          }
        }
      })
    },
    installRemovePackage (name, installed, newVersion) {
      if (name === '') { return }

      let title = installed ? 'Removing package' : 'Installing package'
      let msg = installed ? 'Really remove package' : 'Really install package'
      let cmd = installed ? 'remove' : 'install'

      if (newVersion) {
        title = 'Upgrading package'
        msg = 'Really upgrade package'
        cmd = 'upgrade'
        msg = this.$t(msg, { name: name, ver: newVersion })
      } else {
        msg = this.$t(msg, { name: name })
      }

      title = this.$t(title, { name: name })

      this.$confirm({
        title,
        content: msg,
        onOk: () => {
          this.$spin(this.$t('Waiting for package manager...'))
          this.doInstallRemovePackage(name, cmd).then(r => {
            this.$spin(false)
            this.showStatus(r, title)
          })
        }
      })
    },
    doUpdatePackage () {
      return this.$ubus.call('oui.opkg', 'update')
    },
    updatePackage () {
      this.$spin(this.$t('Waiting for package manager...'))
      this.doUpdatePackage().then(r => {
        this.$spin(false)
        this.showStatus(r, 'Updating package lists')
      }).catch(() => {
        this.$spin(false)
        this.$message.error(this.$t('Updating package lists fail'))
      })
    },
    updateInstalledList () {
      return new Promise(resolve => {
        this.doFetchPackageList('list_installed').then(r => {
          this.installedList = {}
          r.packages.forEach(pkg => {
            this.installedList[pkg.name] = {}
          })

          this.doFetchPackageList('list_upgradable').then(r => {
            r.packages.forEach(pkg => {
              this.installedList[pkg.name].new_version = pkg.new_version
            })
            resolve()
          }).catch(() => {
            resolve()
          })
        })
      })
    }
  },
  watch: {
    displayInstalled () {
      this.fetchPackageList(0)
    },
    currentPage () {
      this.fetchPackageList((this.currentPage - 1) * this.limit)
    }
  },
  created () {
    this.$system.getDiskInfo().then(info => {
      this.diskInfo.used = info.root.used
      this.diskInfo.total = info.root.total
    })

    this.updateInstalledList().then(() => {
      this.fetchPackageList(0)
    })
  }
}
</script>
