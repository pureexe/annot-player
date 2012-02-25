#ifndef _VLCCORE_MODULE_H
#define _VLCCORE_MODULE_H

// vlccore/module.h
// See: vlc/modules.h (1.1.13)
// 2/21/2012

extern "C" {
  #include <vlc/plugins/vlc_common.h>
} // extern "C"

#define MODULE_SHORTCUT_MAX 20

// Must be consistent with the one defined in: vlc/plugins/module.h
typedef void *module_handle_t;
struct module_t
{
  char       *psz_object_name;
  module_t   *next;
  module_t   *submodule;
  module_t   *parent;
  unsigned    submodule_count;
  gc_object_t vlc_gc_data;

  /*
   * Variables set by the module to identify itself
   */
  char *psz_shortname;                              /**< Module name */
  char *psz_longname;                   /**< Module descriptive name */
  char *psz_help;        /**< Long help string for "special" modules */

  /** Shortcuts to the module */
  char *pp_shortcuts[ MODULE_SHORTCUT_MAX ];

  char    *psz_capability;                                 /**< Capability */
  int      i_score;                          /**< Score for the capability */

  bool b_unloadable;                        /**< Can we be dlclosed? */
  bool b_submodule;                        /**< Is this a submodule? */

  /* Callbacks */
  int  ( * pf_activate )   ( vlc_object_t * );
  void ( * pf_deactivate ) ( vlc_object_t * );

  /*
   * Variables set by the module to store its config options
   */
  module_config_t *p_config;             /* Module configuration structure */
  size_t           confsize;            /* Number of module_config_t items */
  unsigned int     i_config_items;        /* number of configuration items */
  unsigned int     i_bool_items;            /* number of bool config items */

  /*
   * Variables used internally by the module manager
   */
  /* Plugin-specific stuff */
  module_handle_t     handle;                             /* Unique handle */
  char *              psz_filename;                     /* Module filename */
  char *              domain;                            /* gettext domain */

  bool          b_builtin;  /* Set to true if the module is built in */
  bool          b_loaded;        /* Set to true if the dll is loaded */
};

#endif // _VLCCORE_VIDEO_H
