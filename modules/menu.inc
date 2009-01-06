<?php
// $Id$

/**
 * @file
 *   Form builder implementation for menu.module.
 */

/**
 * Implementation of hook_form_builder_types().
 */
function menu_form_builder_types() {
  $fields = array();

  // Make a default menu item to pass into the menu form.
  $item = array(
    'mlid' => 0,
    'plid' => 0,
    'weight' => 0,
    'link_title' => '',
    'module' => '',
    'hidden' => NULL,
    'has_children' => NULL,
    'customized' => NULL, 
    'options' => NULL,
    'expanded' => NULL,
    'parent_depth_limit' => NULL,
    'menu_name' => NULL,
  );
  $default = menu_node_form($item);

  // Give the form item a pseudo field type.
  // This is then modified in menu_form_builder_preview_alter().
  $default['menu']['#key'] = 'menu';
  $default['menu']['#type'] = 'menu_settings';

  $fields['menu_settings'] = array(
    'title' => t('Menu'),
    'properties' => array(
      'collapsible',
      'collapsed',
    ),
    'default' => $default['menu'],
    'unique' => TRUE,
    'palette_group' => 'special',
  );

  return array(
    'node' => $fields,
    'example' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_preview_alter().
 *
 * Convert the #type => menu pseudo-element into a real fieldset.
 */
function menu_form_builder_preview_alter(&$element, $form_type, $form_id) {
  if ($element['#type'] == 'menu_settings') {
    $element['#type'] = 'fieldset';
  }
}

/**
 * A straight copy/paste of menu_form_alter(). Get the menu node form.
 *
 * @param $menu_settings
 *   The contents of $node->menu.
 *
 * @todo Put this function directly in menu.module, abstracting it from
 * menu_form_alter().
 */
function menu_node_form($menu_settings) {
  $form = array();

  $form['menu'] = array(
    '#type' => 'fieldset',
    '#title' => t('Menu settings'),
    '#access' => user_access('administer menu'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#attributes' => array('class' => 'menu-item-form'),
  );
  $item = $menu_settings;

  if ($item['mlid']) {
    // There is an existing link.
    $form['menu']['delete'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete this menu item.'),
    );
  }
  if (!$item['link_title']) {
    $form['menu']['#collapsed'] = TRUE;
  }

  foreach (array('mlid', 'module', 'hidden', 'has_children', 'customized', 'options', 'expanded', 'hidden', 'parent_depth_limit') as $key) {
    $form['menu'][$key] = array('#type' => 'value', '#value' => $item[$key]);
  }
  $form['menu']['#item'] = $item;

  $form['menu']['link_title'] = array('#type' => 'textfield',
    '#title' => t('Menu link title'),
    '#default_value' => $item['link_title'],
    '#description' => t('The link text corresponding to this item that should appear in the menu. Leave blank if you do not wish to add this post to the menu.'),
    '#required' => FALSE,
  );
  // Generate a list of possible parents (not including this item or descendants).
  $options = menu_parent_options(menu_get_menus(), $item);
  $default = $item['menu_name'] .':'. $item['plid'];
  if (!isset($options[$default])) {
    $default = 'primary-links:0';
  }
  $form['menu']['parent'] = array(
    '#type' => 'select',
    '#title' => t('Parent item'),
    '#default_value' => $default,
    '#options' => $options,
    '#description' => t('The maximum depth for an item and all its children is fixed at !maxdepth. Some menu items may not be available as parents if selecting them would exceed this limit.', array('!maxdepth' => MENU_MAX_DEPTH)),
    '#attributes' => array('class' => 'menu-title-select'),
  );
  $form['#submit'][] = 'menu_node_form_submit';

  $form['menu']['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#delta' => 50,
    '#default_value' => $item['weight'],
    '#description' => t('Optional. In the menu, the heavier items will sink and the lighter items will be positioned nearer the top.'),
  );

  return $form;
}


/**
 * Implementation of hook_form_builder_load().
 */
function menu_form_builder_load($form_type, $form_id) {
  if ($form_type == 'node') {
    $form['menu']['#form_builder'] = array(
      'element_id' => 'menu',
      'configurable' => TRUE,
      'removable' => TRUE,
      'unique' => TRUE,
    );
    $form['menu']['#type'] = 'menu_settings';

    return $form;
  }
}

/**
 * Implementation of hook_form_builder_save().
 */
function menu_form_builder_save(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $element = form_builder_get_element($form, 'menu');
    $settings = array(
      'collapsible' => $element['#collapsible'],
      'collapsed' => $element['#collapsed'],
      'weight' => $element['#weight'],
    );
    variable_set('menu_settings_' . $form_type, $settings);
  }
}


/**
 * Implementation of hook_form_alter().
 *
 * @todo Merge this into menu_form_alter().
 */
function menu_form_builder_form_alter(&$form, &$form_state, $form_id) {
  // Set properties of the menu form item based on the menu node form settings.
  if (preg_match('/_node_form$/', $form_id) && isset($form['menu'])) {
    $defaults = array(
      'collapsible' => TRUE,
      'collapsed' => TRUE,
      'weight' => -2,
    );

    $menu_settings = variable_get('menu_settings_' . $form['#node']->type, $defaults);
    $form['menu']['#collapsible'] = $menu_settings['collapsible'];
    $form['menu']['#collapsed'] = $menu_settings['collapsed'];
    $form['menu']['#weight'] = $menu_settings['weight'];
  }
}
