export default {
  name: 'UciSectionTableActionUi',
  props: {
    section: Object,
    sid: String
  },
  methods: {
    handleSort(up) {
      const sids = this.section.sids;
      let index = sids.indexOf(this.sid);

      if (up)
        index--;
      else
        index++;

      if (index < 0 || index === sids.length)
        return;

      const nsid = sids[index];
      this.section.swap(this.sid, nsid);
    }
  },
  render(h) {
    const self = this;

    if (this.section.$scopedSlots.action)
      return this.section.$scopedSlots.action({s: this.section, sid: this.sid});

    const delBtn = h('el-button', {
      props: {
        type: 'danger',
        size: 'mini'
      },
      on: {
        click: () => {
          self.section.del(self.sid);
        }
      }
    }, self.$t('Delete'));

    if (this.section.sortable) {
      const upBtn = h('el-button', {
        props: {
          type: 'success',
          size: 'mini'
        },
        on: {
          click: () => {
            self.handleSort(true);
          }
        }
      }, '↑');

      const downBtn = h('el-button', {
        props: {
          type: 'success',
          size: 'mini'
        },
        on: {
          click: () => {
            self.handleSort(false);
          }
        }
      }, '↓');

      return h('el-button-group', {
      }, [upBtn, downBtn, delBtn]);
    } else {
      return delBtn;
    }
  }
}
