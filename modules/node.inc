<?php
// $Id$

/**
 * @file
 *   Form builder implementation for node.module.
 */

/**
 * Implementation of hook_menu().
 *
 * Note: This would be merged into node_menu().
 */
function node_menu_alter(&$items) {
  foreach (node_get_types('types', NULL, TRUE) as $type) {
    $type_url_str = str_replace('_', '-', $type->type);
    $items['admin/content/node-type/'. $type_url_str .'/fields'] = array(
      'title' => 'Fields',
      'page callback' => 'node_form_edit',
      'page arguments' => array($type),
      'access arguments' => array('administer content types'),
      'file' => 'node.pages.inc',
      'file path' => drupal_get_path('module', 'node'),
      'type' => MENU_LOCAL_TASK,
    );
    $items['admin/content/node-type/'. $type_url_str .'/fields/confirm'] = array(
      'title' => 'Confirm field changes',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('node_form_builder_save_form', $type, TRUE),
      'access arguments' => array('administer content types'),
      'type' => MENU_CALLBACK,
    );
  }
}

/**
 * Main page for editing a content type form.
 *
 * @param $type
 *   The content type object being edited.
 */
function node_form_edit($type) {
  module_load_include('inc', 'form_builder', 'includes/form_builder.admin');

  // TODO: These should be auto-loaded as necessary.
  drupal_add_js('misc/autocomplete.js');
  drupal_add_js('misc/collapse.js');
  drupal_add_js('misc/tableheader.js');
  drupal_add_js('misc/textarea.js');

  drupal_set_message(t('This is a form preview. Click on a field to edit its properties.'), 'warning');

  $output = form_builder_interface('node', $type->type);
  $output .= drupal_get_form('node_form_builder_save_form', $type);
  return $output;
}

/**
 * Form for saving Form Builder changes.
 */
function node_form_builder_save_form($form_state, $type, $review = FALSE) {
  module_load_include('inc', 'form_builder', 'includes/form_builder.api');
  module_load_include('inc', 'form_builder', 'includes/form_builder.cache');

  $form = array();

  if ($review) {
    // Allow modules to review dangerous changes before continuing.
    $differences = form_builder_cache_difference('node', $type->type);
    if ($differences !== FALSE) {
      $form['warnings'] = array(
        '#tree' => TRUE,
      );

      if (empty($form_state['post'])) {
        drupal_set_message(t('Please review your changes before saving.'));
      }

      $changed_fields = array();
      foreach ($differences as $difference) {
        $field_name = isset($difference['original']['#title']) ? $difference['original']['#title'] : $difference['original']['#form_builder']['element_id'];
        if (empty($difference['original'])) {
          $field_name = isset($difference['modified']['#title']) ? $difference['modified']['#title'] : $difference['modified']['#form_builder']['element_id'];
          $changed_fields[] = $field_name . ' - <strong>' . t('Added') . '</strong>';
        }
        elseif (empty($difference['modified'])) {
          $changed_fields[] = $field_name . ' - <strong>' . t('Removed') . '</strong>';
        }
        else {
          $changed_fields[] = $field_name . ' - <strong>' . t('Modified') . '</strong>';
        }
      }

      $form['changed'] = array(
        '#type' => 'markup',
        '#value' => theme('item_list', $changed_fields),
      );

      foreach ($differences as $difference) {
        $element_id = $difference['original']['#form_builder']['element_id'];
        $correction_form = module_invoke_all('form_builder_correction_form', $form_state, 'node', $type->type, $element_id, $difference['original'], $difference['modified']);
        if (!empty($correction_form)) {
          $form['warnings'][$element_id] = $correction_form;
        }
      }

      $form['offline'] = array(
        '#title' => t('Site status'),
        '#type' => 'radios',
        '#options' => array(
          '0' => t('Leave site online'),
          '1' => t('Temporarily take site offline'),
        ),
        '#default_value' => '0',
        '#description' => t('For large sets of changes, it is recommended that you temporarily disable access to your site while data is being saved. After changes are complete, the site will be turned back online. You can change the site status manually be visiting <a href="!url">Site maintenance</a>.', array('!url' => url('admin/settings/site-maintenance'))),
        '#access' => variable_get('site_offline', '0') == '0',
      );

      $form['buttons']['cancel'] = array(
        '#type' => 'markup',
        '#value' => l('Cancel', 'admin/content/node-type/' . str_replace('_', '-', $type->type) . '/fields'),
      );

      $form['reviewed'] = array(
        '#type' => 'value',
        '#value' => TRUE,
      );
    }
    else {
      drupal_not_found();
      exit;
    }
  }

  $form['type'] = array(
    '#type' => 'value',
    '#value' => $type,
  );

  $form['buttons']['#weight'] = 100;

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => -1,
  );

  return $form;
}

/**
 * Submit handler for saving changes to the node form.
 */
function node_form_builder_save_form_submit(&$form, &$form_state) {
  module_load_include('inc', 'form_builder', 'includes/form_builder.api');
  module_load_include('inc', 'form_builder', 'includes/form_builder.cache');
  $type = $form_state['values']['type'];

  if (!isset($form_state['values']['reviewed'])) {
    $form_state['redirect'] = 'admin/content/node-type/' . str_replace('_', '-', $type->type) .'/fields/confirm';
    return;
  }

  // Disable access to the site.
  if ($form_state['values']['offline'] == '1') {
    variable_set('site_offline', '1');
  }

  $form = form_builder_cache_load('node', $type->type);
  form_builder_save_form($form, 'node', $type->type);

  // Re-enable access to the site.
  if ($form_state['values']['offline'] == '1') {
    variable_set('site_offline', '0');
  }

  drupal_set_message(t('The changes to the %type content type form have been saved.', array('%type' => $type->name)));
  $form_state['redirect'] = 'admin/content/node-type/' . str_replace('_', '-', $type->type);

}

/**
 * Implementation of hook_form_builder_types().
 */
function node_form_builder_types() {
  global $user;

  module_load_include('inc', 'node', 'node.pages');

  $fields = array();

  $node = array(
    'uid' => $user->uid,
    'created' => 280321200, // 19 Nov 1978 05:00:00.
    'date' => format_date(280321200, 'custom', 'Y-m-d H:i:s O'),
    'body' => NULL,
    'title' => NULL,
    'format' => NULL,
  );
  $settings = node_form_settings();

  $node['status'] = $settings['options']['status'];
  $node['promote'] = $settings['options']['promote'];
  $node['sticky'] = $settings['options']['sticky'];

  $node = (object) $node;


  $title_form = node_title_field($node, $settings['title']);
  $title_form['#weight'] = $settings['title']['weight'];
  $fields['node_title'] = array(
    'title' => t('Title'),
    'properties' => array(
      'title',
    ),
    'default' => $title_form,
    'unique' => TRUE,
    'palette_group' => 'special',
  );

  $body_form = node_body_field($node, $settings['body']['label'], $settings['body']['min_word_count']);
  $body_form['#weight'] = $settings['body']['weight'];
  $body_form['#body_settings'] = $settings['body'];
  $fields['node_body'] = array(
    'title' => t('Body'),
    'properties' => array(
      'body_settings',
    ),
    'default' => $body_form,
    'unique' => TRUE,
    'palette_group' => 'special',
  );

  $revision_fieldset = node_revision_fieldset($node, $settings['revision']);
  $revision_fieldset['#weight'] = $settings['revision']['weight'];
  $revision_fieldset['#revision_settings'] = $settings['revision'];
  $fields['node_revision'] = array(
    'title' => t('Revisions'),
    'properties' => array(
      'collapsible',
      'collapsed',
      'revision_settings',
    ),
    'default' => $revision_fieldset,
    'unique' => TRUE,
    'palette_group' => 'special',
  );

  $author_fieldset = node_author_fieldset($node, $settings['author']);
  $author_fieldset['#author_settings'] = $settings['author'];
  $author_fieldset['#weight'] = $settings['author']['weight'];
  $author_fieldset['#form_builder']['removable'] = FALSE;
  $fields['node_author'] = array(
    'title' => t('Authoring'),
    'properties' => array(
      'collapsible',
      'collapsed',
    ),
    'default' => $author_fieldset,
    'unique' => TRUE,
    'removable' => FALSE,
    'palette_group' => 'special',
  );

  $options_fieldset = node_options_fieldset($node, $settings['options']);
  $options_fieldset['#options_settings'] = $settings['options'];
  $options_fieldset['#weight'] = $settings['options']['weight'];
  $options_fieldset['#form_builder']['removable'] = FALSE;
  $fields['node_options'] = array(
    'title' => t('Options'),
    'properties' => array(
      'collapsible',
      'collapsed',
      'options_settings',
    ),
    'default' => $options_fieldset,
    'unique' => TRUE,
    'removable' => FALSE,
    'palette_group' => 'special',
  );

  return array(
    'node' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_properties().
 */
function node_form_builder_properties($form_type) {
  if ($form_type == 'node') {
    return array(
      'body_settings' => array(
        'form' => 'node_body_settings_form',
        'submit' => array('node_body_settings_form_submit'),
      ),
      'revision_settings' => array(
        'form' => 'node_revision_settings_form',
      ),
      'options_settings' => array(
        'form' => 'node_options_settings_form',
      ),
    );
  }
}

/**
 * Implementation of hook_form_builder_preview_alter().
 *
 * Update the custom node fields according to the changed node type settings.
 */
function node_form_builder_preview_alter(&$element, $form_type, $form_id) {
  if ($form_type == 'node') {
    switch ($element['#form_builder']['element_id']) {
      case 'node_title':
        // No node title settings yet.
        break;
      case 'node_body':
        $element['body']['#title'] = check_plain($element['#body_settings']['label']);
        $element['body']['#required'] = $element['#body_settings']['min_word_count'] > 0;
        break;
      case 'node_revision':
        $element['revision']['#default_value'] = $element['#revision_settings']['revision'];
        break;
      case 'node_author':
        // No node author settings yet.
        break;
      case 'node_options':
        $element['status']['#default_value'] = $element['#options_settings']['status'];
        $element['promote']['#default_value'] = $element['#options_settings']['promote'];
        $element['sticky']['#default_value'] = $element['#options_settings']['sticky'];
        break;
    }
  }
}

/**
 * Configuration form for the "body_settings" property.
 */
function node_body_settings_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['label'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#parents' => array('body_settings', 'label'),
    '#default_value' => $element['#body_settings']['label'],
    '#required' => TRUE,
    '#weight' => -10,
  );

  $form['min_word_count'] = array(
    '#form_builder' => array('property_group' => 'validation'),
    // Manually set parents since we need this to be in the validation group.
    '#parents' => array('body_settings', 'min_word_count'),
    '#type' => 'select',
    '#title' => t('Minimum number of words'),
    '#default_value' => $element['#body_settings']['min_word_count'],
    '#options' => drupal_map_assoc(array(0, 10, 25, 50, 75, 100, 125, 150, 175, 200)),
    '#description' => t('The minimum number of words for the body field to be considered valid for this content type. This can be useful to rule out submissions that do not meet the site\'s standards, such as short test posts.')
  );

  return $form;
}

/**
 * Configuration form for the "revision_settings" property.
 */
function node_revision_settings_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['revision_settings']['revision'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create new revision by default'),
    '#parents' => array('revision_settings', 'revision'),
    '#default_value' => $element['#revision_settings']['revision'],
  );

  return $form;
}

/**
 * Configuration form for the "options_settings" property.
 */
function node_options_settings_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published by default'),
    '#parents' => array('options_settings', 'status'),
    '#default_value' => $element['#options_settings']['status'],
  );

  $form['promote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Promoted by default'),
    '#parents' => array('options_settings', 'promote'),
    '#default_value' => $element['#options_settings']['promote'],
  );

  $form['sticky'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sticky by default'),
    '#parents' => array('options_settings', 'sticky'),
    '#default_value' => $element['#options_settings']['sticky'],
  );

  return $form;
}

/**
 * Implementation of hook_form_builder_load().
 */
function node_form_builder_load($form_type, $form_id) {
  global $user;

  module_load_include('inc', 'node', 'node.pages');

  if ($form_type == 'node') {
    $form_state = array('submitted' => FALSE);
    $node_type = $form_id;

    $node = array(
      'uid' => $user->uid,
      'created' => 280321200, // 19 Nov 1978 05:00:00.
      'date' => format_date(280321200, 'custom', 'Y-m-d H:i:s O'),
      'name' => (isset($user->name) ? $user->name : ''),
      'type' => $node_type,
      'language' => '',
    );
    $form = node_form($form_state, $node);

    // Add Form Builder settings to configurable node fields.
    $settings = node_form_settings($node_type);

    if (isset($form['title'])) {
      $form['title']['#title_settings'] = $settings['title'];
      $form['title']['#weight'] = $settings['title']['weight'];
      $form['title']['#form_builder'] = array(
        'element_id' => 'node_title',
        'element_type' => 'node_title',
      );
    }

    if (isset($form['body_field'])) {
      $form['body_field']['#body_settings'] = $settings['body'];
      $form['body_field']['#weight'] = $settings['body']['weight'];
      $form['body_field']['#form_builder'] = array(
        'element_id' => 'node_body',
        'element_type' => 'node_body',
      );
    }

    if (isset($settings['revision']['enabled'])) {
      $form['revision_information'] = node_revision_fieldset($form['#node'], $settings['revision']);
      $form['revision_information']['#revision_settings'] = $settings['revision'];
      $form['revision_information']['#form_builder'] = array(
        'element_id' => 'node_revision',
        'element_type' => 'node_revision',
      );
    }

    $form['options'] = node_options_fieldset($form['#node'], $settings['options']);
    $form['options']['#options_settings'] = $settings['options'];
    $form['options']['#form_builder'] = array(
      'element_id' => 'node_options',
      'element_type' => 'node_options',
    );

    $form['author'] = node_author_fieldset($form['#node'], $settings['author']);
    $form['author']['#author_settings'] = $settings['author'];
    $form['author']['#form_builder'] = array(
      'element_id' => 'node_author',
      'element_type' => 'node_author',
    );

    unset($form['buttons']);

    return $form;
  }
}

/**
 * Implementation of hook_form_builder_save().
 */
function node_form_builder_save(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $node_type = $form_id;

    $node_element_ids = array(
      'node_title',
      'node_body',
      'node_revision',
      'node_options',
      'node_author',
    );
    $node_elements = form_builder_get_elements($form, $node_element_ids);
    $settings = node_form_settings($node_type);

    foreach ($node_element_ids as $element_id) {
      $settings_key = str_replace('node_', '', $element_id);
      if (!isset($node_elements[$element_id])) {
        $settings[$settings_key]['enabled'] = FALSE;
      }
      else {
        // Merge most settings from the #settings property.
        $settings[$settings_key]['enabled'] = TRUE;
        foreach ($node_elements[$element_id]['#' . $settings_key . '_settings'] as $key => $setting) {
          $settings[$settings_key][$key] = $setting;
        }
        // Set the weight from the #weight property.
        $settings[$settings_key]['weight'] = $node_elements[$element_id]['#weight'];
        // The title label is just the plain title attribute.
        if ($settings_key == 'title') {
          $settings['title']['label'] = $node_elements[$element_id]['#title'];
        }
      }
    }

    // Update the settings.
    variable_set('node_form_settings_' . $node_type, $settings);

    // Special cases for node type settings.
    $type = node_get_types('type', $node_type);
    $type->has_title = $settings['title']['enabled'];
    $type->title_label = $settings['title']['label'];
    $type->has_body = $settings['body']['enabled'];
    $type->body_label = $settings['body']['label'];
    $type->min_word_count = $settings['body']['min_word_count'];
    node_type_save($type);
  }
}

/**
 * Implementation of hook_form_alter().
 *
 * @todo Merge this into node_form().
 */
function node_form_builder_form_alter(&$form, &$form_state, $form_id) {
  // Set properties of the node form based on the node form settings.
  if (preg_match('/_node_form$/', $form_id)) {
    $type = $form['#node']->type;
    $node_form_settings = node_form_settings($type);

    foreach ($node_form_settings as $key => $settings) {
      if (isset($settings['enabled']) && !$settings['enabled']) {
        $form[$key]['#access'] = FALSE;
      }
      else {
        switch ($key) {
          case 'title':
            $form[$key] = node_title_field($form['#node'], $settings);
            break;
          case 'body':
            $form['body_field']['#weight'] = $settings['weight'];
            break;
          case 'revision':
            $form['revision_information'] = node_revision_fieldset($form['#node'], $settings);
            break;
          case 'author':
            $form[$key] = node_author_fieldset($form['#node'], $settings);
            break;
          case 'options':
            $form[$key] = node_options_fieldset($form['#node'], $settings);
            break;
        }
      }
    }
  }
}

/**
 * A copy/paste out of node_content_form(). The title field.
 */
function node_title_field($node, $title_settings) {
  return array(
    '#type' => 'textfield',
    '#title' => check_plain($title_settings['label']),
    '#required' => TRUE,
    '#default_value' => $node->title,
    '#maxlength' => 255,
    '#weight' => $title_settings['weight'],
  );
}

/**
 * A straight copy/paste of a segment of node_form(). The node revision form.
 *
 * @param $node
 *   The node object being edited.
 * @param $revision_settings
 *   Settings specific to the revision fieldset.
 *
 * @todo Put this function directly in node.pages.inc, abstracting it from
 * node_form().
 */
function node_revision_fieldset($node, $revision_settings) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Revision information'),
    '#collapsible' => TRUE,
    // Collapsed by default when "Create new revision" is unchecked
    '#collapsed' => $revision_settings['collapsed'],
    '#weight' => $revision_settings['weight'],
  );
  $form['revision'] = array(
    '#access' => user_access('administer nodes'),
    '#type' => 'checkbox',
    '#title' => t('Create new revision'),
    '#default_value' => $revision_settings['revision'],
  );
  $form['log'] = array(
    '#type' => 'textarea',
    '#title' => t('Log message'),
    '#rows' => 2,
    '#description' => t('An explanation of the additions or updates being made to help other authors understand your motivations.'),
  );

  return $form;
}

/**
 * A straight copy/paste of a segment of node_form(). The authoring information form.
 *
 * @param $node
 *   The node object being edited.
 * @param $authoring_settings
 *   Settings specific to the authoring information fieldset.
 *
 * @todo Put this function directly in node.pages.inc, abstracting it from
 * node_form().
 */
function node_author_fieldset($node, $authoring_settings) {
  $form = array(
    '#type' => 'fieldset',
    '#access' => user_access('administer nodes'),
    '#title' => t('Authoring information'),
    '#collapsible' => $authoring_settings['collapsible'],
    '#collapsed' => $authoring_settings['collapsed'],
    '#weight' => $authoring_settings['weight'],
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Authored by'),
    '#maxlength' => 60,
    '#autocomplete_path' => 'user/autocomplete',
    '#default_value' => isset($node->name) ? $node->name : '',
    '#weight' => -1,
    '#description' => t('Leave blank for %anonymous.', array('%anonymous' => variable_get('anonymous', t('Anonymous')))),
  );
  $form['date'] = array(
    '#type' => 'textfield',
    '#title' => t('Authored on'),
    '#maxlength' => 25,
    '#description' => t('Format: %time. Leave blank to use the time of form submission.', array('%time' => !empty($node->date) ? $node->date : format_date($node->created, 'custom', 'Y-m-d H:i:s O'))),
  );

  if (isset($node->date)) {
    $form['date']['#default_value'] = $node->date;
  }

  return $form;
}

/**
 * A straight copy/paste of a segment of node_form(). The publishing options form.
 *
 * @param $node
 *   The node object being edited.
 * @param $publishing_settings
 *   Settings specific to the publishing options fieldset.
 *
 * @todo Put this function directly in node.pages.inc, abstracting it from
 * node_form().
 */
function node_options_fieldset($node, $publishing_settings) {
  $form = array(
    '#type' => 'fieldset',
    '#access' => user_access('administer nodes'),
    '#title' => t('Publishing options'),
    '#collapsible' => $publishing_settings['collapsible'],
    '#collapsed' => $publishing_settings['collapsed'],
    '#weight' => $publishing_settings['weight'],
  );
  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published'),
    '#default_value' => $node->status,
  );
  $form['promote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Promoted to front page'),
    '#default_value' => $node->promote,
  );
  $form['sticky'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sticky at top of lists'),
    '#default_value' => $node->sticky,
  );

  return $form;
}

/**
 * Retrieve the default node form settings.
 */
function node_form_settings($node_type = NULL) {
  $defaults = array(
    'title' => array(
      'enabled' => TRUE,
      'label' => t('Title'),
      'weight' => -5,
    ),
    'body' => array(
      'enabled' => TRUE,
      'label' => t('Body'),
      'min_word_count' => 0,
      'weight' => 0,
    ),
    'revision' => array(
      'enabled' => TRUE,
      'collapsible' => TRUE,
      'collapsed' => TRUE,
      'weight' => 20,
      'revision' => FALSE,
    ),
    'author' => array(
      'collapsible' => TRUE,
      'collapsed' => TRUE,
      'weight' => 21,
    ),
    'options' => array(
      'collapsible' => TRUE,
      'collapsed' => TRUE,
      'status' => 1,
      'promote' => 0,
      'sticky' => 0,
      'weight' => 25,
    ),
  );

  if (!isset($node_type)) {
    return $defaults;
  }

  // Merge the variable settings with those from the node_type table.
  $settings = variable_get('node_form_settings_' . $node_type, $defaults);
  if ($type = node_get_types('type', $node_type)) {
    $settings['title']['enabled'] = $type->has_title;
    $settings['title']['label'] = $type->title_label;
    $settings['body']['enabled'] = $type->has_body;
    $settings['body']['label'] = $type->body_label;
    $settings['body']['min_word_count'] = $type->min_word_count;
  }

  return $settings;
}
